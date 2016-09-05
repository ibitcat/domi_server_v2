/******************************************************************** 
创建时间:        2015/06/22 14:50
文件名称:        singleton.h
文件作者:        Domi
功能说明:        单例模板  
其他说明:         
*********************************************************************/

#pragma once
#include <iostream>
using namespace std;
//#include "system/basic/basicTypes.h"

// 第一种实现
template <typename T>
class Singleton
{
protected:
	Singleton(){}
	~Singleton(){}

public:
	inline static T* GetSingletonPtr()	{
		static T instance;
		return &instance;;	
	}

	inline static T& GetSingleton(){
		return *(Singleton::GetSingletonPtr());
	}
};


/*
// 第二种实现
template <typename T>
class Singleton
{
private:
	static T* m_pInstance;

	// GC
	//它的唯一工作就是在析构函数中删除CSingleton的实例 
	template<class M>
	class CGarbo 
	{
	public:
		CGarbo() { }
		~CGarbo()
		{
			if (ptr)
				delete ptr;
		}

		M* ptr;
	};

protected:
	Singleton(){}
	virtual ~Singleton(){}

public:
	inline static T* GetSingletonPtr()
	{
		if (m_pInstance == nullptr)  //判断是否第一次调用  
		{
			static CGarbo<T> garbo;
			garbo.ptr = new T();
			m_pInstance = garbo.ptr;
		}

		return m_pInstance;
	}

	inline static T& GetSingleton()
	{
		return *(Singleton::GetSingletonPtr());
	}
};

//template<class T>
//typename Singleton<T>::CGarbo Singleton<T>::Garbo;

template<class T> 
T* Singleton<T>::m_pInstance = nullptr;
*/