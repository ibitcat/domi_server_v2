/******************************************************************** 
创建时间:        2015/07/06 17:03
文件名称:        map_pool.h
文件作者:        Domi
功能说明:        map pool
其他说明:		 map 分配池 
*********************************************************************/

#pragma once
#include <map>
#include "../thread/csLocker.h"

template <typename Key,typename T,int COUNT = 0>
class CMapPool
{
private:
	typedef	std::map<Key,T*> HASH_MAP;
	typedef std::queue<T*> QUEUE;

public:
	typedef	typename HASH_MAP::iterator	iterator;

public:
	CMapPool();
	virtual ~CMapPool();

private:
	CMutex				m_csLock;
	HASH_MAP			m_UseList;

	volatile int		m_nCount;
	volatile int		m_nMaxFree;
	QUEUE				m_FreeList;		// 空闲队列


public:
	inline CMutex&	getLock		()			 {	return m_csLock;			}
	inline void		setMaxFree	(int nCount) {	m_nMaxFree = nCount;		}
	inline void		setPoolCount(int nCount) {	m_nCount = nCount;			}
	inline iterator	begin		()			 {	return m_UseList.begin();	}
	inline iterator	end			()			 {	return m_UseList.end();		}
	inline bool		empty		()const		 {	return m_UseList.empty();	}
	inline size_t	size		()const 	 {	return m_UseList.size();	} 

private:
	inline void	_build	();
	inline T*	_malloc	();
	inline void	_free	(T* p_t);

public:
	void	initPool();

public:
	T*		allocate(const Key& key);
	bool	release	(const Key& key);
	bool	release	(iterator& pos);
	void	clear	();
	void	getHead	(iterator& pos);
	void	getEnd	(iterator& pos);
	T*		getHead	();
	T*		getNext	(iterator& pos);
	T*		getData	(const iterator& pos);
	T*		find	(const Key& key);
	bool	isTail	(const iterator& pos);
};

//-----------------------------------------------------------------------
template<typename Key,class T,int COUNT>
CMapPool<Key,T,COUNT>::CMapPool(){
	m_nCount	= COUNT;
	m_nMaxFree	= COUNT;
}
 
template<typename Key,class T,int COUNT>
CMapPool<Key,T,COUNT>::~CMapPool(){
	initPool();
}
  
template<typename Key,class T,int COUNT>
inline void	CMapPool<Key,T,COUNT>::_build(){
	T*	lp_t = nullptr;
	try{
		/*分配m_nCount个内存块*/ 
		for( int i = 0; i < m_nCount; i++ ){
			lp_t = new T;
			m_FreeList.push(lp_t);
		}/*end for(i)*/ 
	}catch (...){
	}
}

template<typename Key,class T,int COUNT>
inline T* CMapPool<Key,T,COUNT>::_malloc(){
	T* p_t = nullptr;
	try{
		if (m_FreeList.empty()) _build();
		if(!m_FreeList.empty()){
			p_t = m_FreeList.front();
			m_FreeList.pop();
		}
		if(!p_t) p_t = new T;
	}catch(...){
	}
	return p_t;
}
 
template<typename Key,class T,int COUNT>
void CMapPool<Key,T,COUNT>::_free(T* p_t){
	if (!p_t) return;
	try{
		if(m_nMaxFree > 0 && (int)m_FreeList.size() > m_nMaxFree)
			delete p_t;
		else
			m_FreeList.push(p_t);
	}catch(...){
	}
}
 
// 清空map
template<typename Key,class T,int COUNT>
void CMapPool<Key,T,COUNT>::initPool(){
	try{
		T*	lp_t = nullptr;
		iterator pos;
		for(pos = m_UseList.begin();pos != m_UseList.end();++pos){
			if (pos->second) delete pos->second;
		}

		while(!m_FreeList.empty()){
			lp_t = m_FreeList.front();
			m_FreeList.pop();
			if(lp_t) delete lp_t;
		}
	}catch(...){
	}
	m_UseList.clear();
}

// 分配空间
template<typename Key,class T,int COUNT>
T* CMapPool<Key,T,COUNT>::allocate(const Key& key){
	T*	lp_t = nullptr;
	try{
		/*避免重复Key再次分配*/ 
		iterator _pos = m_UseList.find(key);
		if (_pos != m_UseList.end()){
			lp_t = _pos->second;
		}
		if (!lp_t){
			lp_t = _malloc();
			if (lp_t) m_UseList.insert(std::make_pair(key,lp_t));
		}
	}catch (...){
	}
	return lp_t;
}

template<typename Key,class T,int COUNT>
bool CMapPool<Key,T,COUNT>::release(const Key& key){
	T* lp_t = nullptr;
	bool bRet = true;
	iterator pos;
	try{
		pos = m_UseList.find(key);
		if (pos != m_UseList.end()){
			lp_t = pos->second;
			m_UseList.erase(pos);
			_free(lp_t);
		}else {
			bRet = false;
		}	
	}catch(...){
		return false;
	}
	return bRet;
}
 
template<typename Key,class T,int COUNT>
bool CMapPool<Key,T,COUNT>::release(iterator& pos){
	T* lp_t = nullptr;
	bool bRet = true;
	try{
		if (pos != m_UseList.end()){
			lp_t = pos->second;
			pos = m_UseList.erase_( pos );
			_free(lp_t);
		}else {
			bRet = false;
		}
	}catch (...){
		return false;
	}
	return bRet;
} 
 
template<typename Key,class T,int COUNT>
void CMapPool<Key,T,COUNT>::clear(){
	T* lp_t	= nullptr;
	iterator pos;
	try{
		pos = m_UseList.begin();
		for ( ; pos != m_UseList.end(); ++pos ){
			lp_t = pos->second;
			_free(lp_t);
		}
		m_UseList.clear();
	}catch (...){
	}
} 
 
template<typename Key,class T,int COUNT>
void CMapPool<Key,T,COUNT>::getHead(iterator& pos){
	try{
		pos = m_UseList.begin();
	}catch (...){
	}
} 
 
template<typename Key,class T,int COUNT>
void CMapPool<Key,T,COUNT>::getEnd(iterator& pos)
{
	try{
		pos = m_UseList.end();
	}catch (...){
	}
}
  
template<typename Key,class T,int COUNT>
T* CMapPool<Key,T,COUNT>::getHead()
{
	T*	lp_t = nullptr;
	try{
		iterator pos = m_UseList.begin();
		if(pos != m_UseList.end()) lp_t = pos->second;
	}catch (...){
	}
	return lp_t;
}
 
template<typename Key,class T,int COUNT>
T* CMapPool<Key,T,COUNT>::getNext(iterator& pos){
	T*	lp_t = nullptr;
	try{
		if(pos != m_UseList.end()){
			lp_t = pos->second;
			++pos;
		}
	}catch (...){
	}
	return lp_t;
}
  
template<typename Key,class T,int COUNT>
T* CMapPool<Key,T,COUNT>::getData(const iterator& pos)
{
	T*	lp_t = nullptr;
	try{
		if (pos != m_UseList.end()) lp_t = pos->second;
	}catch (...){
	}
	return lp_t;
}
  
template<typename Key,class T,int COUNT>
T* CMapPool<Key,T,COUNT>::find(const Key& key){
	T*	lp_t = nullptr;
	iterator pos;
	try{
		pos = m_UseList.find(key);
		if (pos != m_UseList.end()) lp_t = pos->second;
	}catch (...){
	}
	return lp_t;
}
  
template<typename Key,class T,int COUNT>
bool CMapPool<Key,T,COUNT>::isTail(const iterator& pos)
{
	bool bRes=true;
	try{
		if (pos != m_UseList.end()) bRes = false;
	}catch(...){
	}
	return bRes;
}