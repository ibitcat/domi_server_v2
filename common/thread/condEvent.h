#pragma once
#include "../basic/basicTypes.h"
#ifdef WIN32
#include <winsock2.h>
#include <windows.h>
#else//linux
#include <sys/types.h>//系统类型定义
#include <signal.h>
#endif//WIN32
#include "csLocker.h"

#ifndef INFINITE
#define INFINITE	0xFFFFFFFF
#endif

class CCondEvent
{
protected:
	bool m_bCreated;	// 事件对象是否已创建
#ifdef WIN32
	HANDLE				m_hEvent;	//win32中为事件句柄
#else//linux
	//linux中使用互斥锁与条件变量结合
	pthread_cond_t		m_pthread_cond;
	CMutex				m_clLocker;
	bool				m_manual_reset;	//手动初始化
	bool				m_signaled;		//是否已触发信号
#endif//WIN32

public:
	CCondEvent();
	~CCondEvent();

public:
	// 事件是否有效
	inline bool	isValid() { return m_bCreated; }

	bool create_event(bool bManualReset,bool bInitialState);	// 创建事件
	void close_event();	// 关闭事件
	void set_event();	// 触发事件
	void reset_event();	// 复位事件
	bool wait_event(uint64 milliseconds);	// 等待事件[返回false标示超时]
};