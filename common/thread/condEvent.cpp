#ifndef WIN32
#include <sys/time.h> 
#endif//WIN32
#include "condEvent.h"

CCondEvent::CCondEvent(){
	m_bCreated		= false;
#ifdef WIN32
	m_hEvent		= INVALID_HANDLE_VALUE;
#else//linux
	m_manual_reset	= false;
	m_signaled		= false;
#endif//WIN32
}

CCondEvent::~CCondEvent(){
	close_event();
}

// 创建事件
// @bManualReset  是否手动复原事件  true=需要手动复原  false= 当线程被释放后，事件将复原到无信号状态
// @bInitialState 指定事件对象的初始状态。如果为TRUE，初始状态为有信号状态；否则为无信号状态。
bool CCondEvent::create_event(bool bManualReset,bool bInitialState){
	close_event();
#ifdef WIN32
	m_hEvent = ::CreateEvent(nullptr,bManualReset,bInitialState,nullptr);
	if (m_hEvent == INVALID_HANDLE_VALUE) return false;
	::ResetEvent(m_hEvent);
#else//linux
	if(pthread_cond_init(&m_pthread_cond,nullptr) != 0) return false;
	m_manual_reset	= bManualReset;
	m_signaled		= bInitialState;
#endif//WIN32
	m_bCreated = true;
	return true;
}

// 关闭事件
void CCondEvent::close_event(){
	if(!m_bCreated) return;
#ifdef WIN32
	::CloseHandle(m_hEvent);
	m_hEvent = INVALID_HANDLE_VALUE;
#else//linux
	pthread_cond_destroy(&m_pthread_cond);
#endif//WIN32
	m_bCreated = false;
}

// 触发事件
void CCondEvent::set_event(){
	if(!m_bCreated) return;
#ifdef WIN32
	::SetEvent(m_hEvent);	// 设置为有信号状态
#else//linux
	CCritLocker clLock(m_clLocker);
	if(m_manual_reset)
		pthread_cond_broadcast(&m_pthread_cond);//解除阻塞所有线程
	else
		pthread_cond_signal(&m_pthread_cond);//解除阻塞特定线程
	
	m_signaled = true;
#endif//WIN32
}

// 复位事件
void CCondEvent::reset_event(){
	if(!m_bCreated) return;

#ifdef WIN32
	::ResetEvent(m_hEvent);		// 复原事件到无信号状态
#else//linux
	CCritLocker clLock(m_clLocker);
	m_signaled = false;
#endif//WIN32
}

// 等待事件[返回false标示超时]
bool CCondEvent::wait_event(uint64 milliseconds){
	if(!m_bCreated) return true;

#ifdef WIN32
	if(::WaitForSingleObject(m_hEvent,(DWORD)milliseconds) == WAIT_TIMEOUT)	// 等待信号，没有就一直阻塞
		return false;
#else//linux
	//需要使用lock wait unlock方法使用
	CCritLocker clLock(m_clLocker);
	if(m_signaled){
		//不需要手动复位
		if (!m_manual_reset) m_signaled = false;
		return true;
	}

	//未设置超时时间
	if(!milliseconds) return false;
	timeval _now;
	gettimeofday(&_now,nullptr);

	timespec _abstime;
	_abstime.tv_sec = _now.tv_sec + milliseconds / 1000 + (((milliseconds % 1000) * 1000 + _now.tv_usec) / 1000000);
	_abstime.tv_nsec= (((milliseconds % 1000) * 1000 + _now.tv_usec) % 1000000) * 1000;

	int ret = 0;
	do{
		if(milliseconds != INFINITE)
			ret = pthread_cond_timedwait(&m_pthread_cond,m_clLocker.get_mutex(),&_abstime);
		else
			ret = pthread_cond_wait(&m_pthread_cond,m_clLocker.get_mutex());

	} while (ret == 0 && !m_signaled);

	//超时
	if(ret == ETIMEDOUT) return false;
#endif//WIN32
	return true;
}


/*TODO::条件变量属性
 条件变量始终与互斥锁一起使用
如果条件为假，线程通常会基于条件变量阻塞，并以原子方式释放等待条件变化的互斥锁。如果另一个线程更改了条件，该线程可能会向相关的条件变量发出信号，从而使一个或多个等待的线程执行以下操作：
	唤醒
	再次获取互斥锁
	重新评估条件

 在以下情况下，条件变量可用于在进程之间同步线程：
	线程是在可以写入的内存中分配的
	内存由协作进程共享

条件变量属性必须首先由 pthread_condattr_destroy 重新初始化后才能重用
*/
/*
初始化条件变量属性	int	pthread_condattr_init(pthread_condattr_t *cattr);//cattr范围可能的值为 PTHREAD_PROCESS_PRIVATE[缺省值] 和 PTHREAD_PROCESS_SHARED
删除条件变量属性	int	pthread_condattr_destroy(pthread_condattr_t *cattr);
设置条件变量的范围	int	pthread_condattr_setpshared(pthread_condattr_t *cattr,int pshared);//PTHREAD_PROCESS_SHARED/PTHREAD_PROCESS_PRIVATE
获取条件变量的范围	int	pthread_condattr_getpshared(const pthread_condattr_t *cattr,int *pshared);
*/
/*
Solaris			POSIX							定义 
USYNC_PROCESS	PTHREAD_PROCESS_SHARED			用于同步该进程和其他进程中的线程 
USYNC_THREAD	PTHREAD_PROCESS_PRIVATE			用于仅同步该进程中的线程
*/

/*TODO:使用条件变量
初始化条件变量			int	pthread_cond_init(pthread_cond_t *cv,const pthread_condattr_t *cattr);//cattr可为NULL=缺省
基于条件变量阻塞		int	pthread_cond_wait(pthread_cond_t *cv,pthread_mutex_t *mutex);//阻塞的线程可以通过 pthread_cond_signal() 或 pthread_cond_broadcast() 唤醒，也可以在信号传送将其中断时唤醒
在指定的时间之前阻塞	int	pthread_cond_timedwait(pthread_cond_t *cv,pthread_mutex_t *mp, const struct timespec *abstime);//ETIMEDOUT=超时
在指定的时间间隔内阻塞	int pthread_cond_reltimedwait_np(pthread_cond_t *cv,pthread_mutex_t *mp,const struct timespec *reltime);
解除阻塞特定线程		int	pthread_cond_signal(pthread_cond_t *cv);
解除阻塞所有线程		int	pthread_cond_broadcast(pthread_cond_t *cv);
销毁条件变量状态		int	pthread_cond_destroy(pthread_cond_t *cv);

******
唤醒丢失问题
1 一个线程调用 pthread_cond_signal() 或 pthread_cond_broadcast() 
2 另一个线程已经测试了该条件，但是尚未调用 pthread_cond_wait() 
3 没有正在等待的线程
4 信号不起作用，因此将会丢失
*/
