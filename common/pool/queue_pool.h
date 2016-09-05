/******************************************************************** 
创建时间:        2015/07/01 12:02
文件名称:        CQueuePool.h
文件作者:        Domi
功能说明:        队列池  
其他说明:        可循环利用
*********************************************************************/

#pragma once
#include <queue>
#include "../thread/csLocker.h"

template<class _Ty>
class CQueuePool
{
protected:
	typedef std::queue<_Ty*> QUEUE;
	enum{ _block_max = 10000 };	//每次申请块最大尺寸

public:
	CQueuePool();
	virtual~CQueuePool();

protected:
	inline void	_build();
	inline _Ty*	_malloc();
	inline void	_free(_Ty*_val);

public:
	inline bool		empty			()const		{	return m_use_queue.empty();	}
	inline size_t	size			()const		{	return m_use_queue.size();	}
	inline void		set_block_size	(int _size)	{	m_block_size = _size;		}
	inline CMutex&	get_mutex		()			{	return m_mutex;				}

	inline void	destroy();
	inline void	clear();
	inline void	pop();
	inline _Ty*	push();
	inline _Ty*	front();

public:
	CMutex	m_mutex;		//线程安全互斥锁
	int		m_block_size;	//每次申请块大小(默认1,最大10000)
	QUEUE	m_use_queue;	//使用队列
	QUEUE	m_free_queue;	//释放队列
};

//-----------------------------------------------------------------------
template<class _Ty>
CQueuePool<_Ty>::CQueuePool(){
	m_block_size = 1;
}

template<class _Ty>
CQueuePool<_Ty>::~CQueuePool(){
	destroy();
}

template<class _Ty>
inline void	CQueuePool<_Ty>::_build(){
	//调整尺寸
	if(m_block_size < 1)
		m_block_size = 1;

	if(m_block_size > _block_max)
		m_block_size = _block_max;

	try{
		_Ty*_val = nullptr; //分配对像内存
		for (int i = 0;i < m_block_size;i++){
			_val = new _Ty;
			if(_val) m_free_queue.push(_val);
		}
	}catch (...){
		//printf("%s Crash!\n", "CQueuePool");
	}
}

// 分配空间
// 如果用free空间，则从free空间取出，反之，重新new
template<class _Ty>
inline _Ty*	CQueuePool<_Ty>::_malloc(){
	_Ty*_val = nullptr;
	try{
		if(m_free_queue.empty())
			_build();

		if(!m_free_queue.empty()){
			_val = m_free_queue.front();
			m_free_queue.pop();
		}
	}catch (...){
	}
	return _val;
}

// 释放空间，使之成为空闲
template<class _Ty>
inline void	CQueuePool<_Ty>::_free(_Ty*_val){
	try{
		if(_val) m_free_queue.push(_val);
	}catch (...){
	}
}

// 销毁队列池，清空已经使用的和空闲的
template<class _Ty>
inline void	CQueuePool<_Ty>::destroy(){
	try{
		_Ty*_val = nullptr;
		//销毁使用队列
		while(!m_use_queue.empty()){
			_val = m_use_queue.front();
			m_use_queue.pop();
			if(_val) delete _val;
		}

		//销毁释放队列
		while(!m_free_queue.empty()){
			_val = m_free_queue.front();
			m_free_queue.pop();
			if(_val) delete _val;
		}
	}catch (...){
	}
}

//清空使用队列，丢到free里面
template<class _Ty>
inline void	CQueuePool<_Ty>::clear(){
	try{
		while(!m_use_queue.empty()){
			_free(m_use_queue.front());
			m_use_queue.pop();
		}
	}catch (...){
	}
}

// 从使用队列中pop出一个元素
template<class _Ty>
inline void	CQueuePool<_Ty>::pop(){
	try{
		if(!m_use_queue.empty()){
			_free(m_use_queue.front());
			m_use_queue.pop();
		}
	}catch (...){
	}
}

// 压入一个元素
template<class _Ty>
inline _Ty*	CQueuePool<_Ty>::push()
{
	_Ty*_val = nullptr;
	try{
		_val = _malloc();
		if(_val) m_use_queue.push(_val);
	}catch (...){
	}
	return _val;
}
 
// 返回队列头部元素
// 一般先调用front()，取出元素并完成某些操作后，在pop()
template<class _Ty>
inline _Ty*	CQueuePool<_Ty>::front(){
	_Ty*_val = nullptr;
	try{
		if(!m_use_queue.empty()) _val = m_use_queue.front();
	}catch (...){
	}
	return _val;
}
