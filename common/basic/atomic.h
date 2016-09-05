/******************************************************************** 
创建时间:        2015/07/09 16:07
文件名称:        atomic.h
文件作者:        Domi
功能说明:        原子操作封装  
其他说明:         
*********************************************************************/

#pragma once

#ifdef WIN32
#include <Windows.h>
#else
#include<pthread.h>
#include<semaphore.h>
#endif

template<class T>
class CAtomicInteger
{
private:
	volatile T value;
public:
	CAtomicInteger()
		:value(0){
	}
	~CAtomicInteger() {}

public:
	// 原子操作
	T AtomicAdd(T t);
	T AtomicInc();
	T AtomicDec();

	inline T IncrementAndGet()	{ return AtomicInc(); }
	inline T GetValue()			{ return value; }
};

//-------------------------------------------------------------------
// 原子操作加，如果t为负值，则是原子操作减
template < class T >
T CAtomicInteger<T>::AtomicAdd( T t){
#ifdef WIN32
	return (T)InterlockedExchangeAdd((T*)&value, (T)t);
#else
	//	return __gnu_cxx::__exchange_and_add(pt, t);
	return __sync_add_and_fetch(&value, t);
	//return *++pt;
#endif
}

// 原子操作 ++
template < class T >
T CAtomicInteger<T>::AtomicInc(){
#ifdef WIN32
	return (T)InterlockedIncrement((T*)&value);
#else
	//return __gnu_cxx::__exchange_and_add(pt, 1);
	return __sync_add_and_fetch(&value, 1);
#endif
}

// 原子操作 --
template < class T >
T CAtomicInteger<T>::AtomicDec(){
#ifdef WIN32
	return (T)InterlockedDecrement((T*)&value);
#else
	//return __gnu_cxx::__exchange_and_add(pt, -1);
	return __sync_add_and_fetch(&value, -1);
#endif
}