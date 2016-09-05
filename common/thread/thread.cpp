#include "thread.h"
#ifndef WIN32
#include<sys/types.h>
#include<errno.h>
#endif//WIN32

CThread::CThread()
{
#ifdef WIN32
	m_hThread		= INVALID_HANDLE_VALUE;
	m_uThreadID		= 0;
#else//linux
	m_pthreadID			= 0;
	//初始化线程属性获得
	m_iInit = pthread_attr_init(&m_threadAtrr);
	/*默认值
	scope			PTHREAD_SCOPE_PROCESS 新线程与进程中的其他线程发生竞争。 
	detachstate 	PTHREAD_CREATE_JOINABLE 线程退出后，保留完成状态和线程 ID。
	stackaddr		NULL 新线程具有系统分配的栈地址。 
	stacksize		0 新线程具有系统定义的栈大小。 
	priority		0 新线程的优先级为 0。 
	inheritsched	PTHREAD_EXPLICIT_SCHED 新线程不继承父线程调度优先级。 
	schedpolicy		SCHED_OTHER 新线程对同步对象争用使用 Solaris 定义的固定优先级。线程将一直运行，直到被抢占或者直到线程阻塞或停止为止。 
	*/
#endif//WIN32
}

CThread::~CThread(){
	shutdown();
#ifndef WIN32
	//销毁线程属性
	pthread_attr_destroy(&m_threadAtrr);
#endif//WIN32
}

//获得当前线程ID
int64	CThread::getCurrentThreadID(){
#ifdef WIN32
	return int64(GetCurrentThreadId());
#else//linux
	return int64(pthread_self());
#endif//WIN32
}

#ifdef WIN32
bool CThread::create(unsigned(__stdcall*start_routine)(void*),void*arg){
	shutdown();
	m_hThread = (HANDLE)::_beginthreadex(nullptr
										,0
										,start_routine	 /*工作线程函数*/ 
										,arg
										,0
										,&m_uThreadID);
	if (m_hThread == INVALID_HANDLE_VALUE){
		shutdown();
		return false;
	}
	return true;
}
#else//linux
bool CThread::create(void*(*start_routine)(void*),void*arg){
	shutdown();
	int32 _ret = pthread_create(&m_pthreadID
							,&m_threadAtrr	//tattr=NULL为缺省线程
							,start_routine	//线程运行函数指针【此函数退出线程结束(其退出状态设置为由 start_routine 返回的值)】
							,arg);			//线程运行函数参数

	//_ret == EAGAIN///系统限制创建新的线程
	//_ret == EINVAL///第二个参数代表的线程属性值非法
	return (_ret == 0);
}
#endif//WIN32

void CThread::shutdown(){
	terminate();
	wait_exit();
}

//终止线程
int32 CThread::terminate(uint64 exit_code){
	int32 iRet = 0;
#ifdef WIN32
	if(m_hThread != INVALID_HANDLE_VALUE && m_hThread != 0)
		iRet = ::TerminateThread(m_hThread,(DWORD)exit_code);
#else//WIN32
	if(pthread_equal(m_pthreadID,0) == 0)
		iRet =  pthread_cancel(m_pthreadID);
#endif//WIN32
	return iRet;
}

// 等待线程退出
int32 CThread::wait_exit(){
	int32 iRet = 0;
#ifdef WIN32
	if(m_hThread != nullptr && m_hThread != INVALID_HANDLE_VALUE){
		//等待线程终止【会一直阻塞调用线程，直到指定的线程终止】
		::WaitForSingleObject(m_hThread,10*1000/*INFINITE*/);
		/*获得线程编号*/
		DWORD	dwExit = 0;
		::GetExitCodeThread(m_hThread,&dwExit);
		//dwExit == STILL_ACTIVE;//仍然运行中

		::CloseHandle( m_hThread );		/*关闭句柄*/ 
		m_hThread = INVALID_HANDLE_VALUE;
	}

#else//WIN32
	if(pthread_equal(m_pthreadID,0) == 0){
		//等待线程终止【会一直阻塞调用线程，直到指定的线程终止】
		iRet = pthread_join(m_pthreadID	//被等待的线程标识符【在当前进程中且不得是分离线程】
			,nullptr/*void **status*/);//线程退出状态【一般使用NULL】

        //非阻塞式分离
		//pthread_detach(m_pthreadID);
	}
	m_pthreadID = 0;
#endif//WIN32
	return iRet;
}

//是否运行中
bool CThread::is_runing(){
	bool _runing = false;
#ifdef WIN32
	/*获得线程编号*/
	DWORD	dwExit = 0;
	::GetExitCodeThread(m_hThread,&dwExit);
	if (dwExit == STILL_ACTIVE && ::WaitForSingleObject(m_hThread,1) == WAIT_TIMEOUT)
		_runing = true;

#else//WIN32
	if(pthread_equal(m_pthreadID,0) == 0){
		int pthread_kill_err;
		pthread_kill_err = pthread_kill(m_pthreadID,0);

		if(pthread_kill_err != ESRCH && pthread_kill_err != EINVAL)
			_runing = true;
	}
#endif//WIN32
	return _runing;
}

//设置线程的优先级
/*
policy = SCHED_OTHER
sched_param.sched_priority = priority;
*/
int32 CThread::set_sched_param(int32 policy,const struct sched_param *param){
	int32 iRet = 0;
#ifdef WIN32
	iRet = SetThreadPriority(m_hThread,policy);
#else//WIN32
	if(pthread_equal(m_pthreadID,0) == 0)
		iRet =  pthread_setschedparam(m_pthreadID,policy,param);
#endif//WIN32
	return iRet;
}

int32 CThread::get_sched_param(int32*policy,struct sched_param *param){
	int32 iRet = 0;
#ifdef WIN32
	if(policy) *policy = GetThreadPriority(m_hThread);
#else//WIN32
	if(pthread_equal(m_pthreadID,0) == 0)
		iRet =  pthread_getschedparam(m_pthreadID,policy,param);
#endif//WIN32
	return iRet;
}

//向线程发送信号
/*
信号		取值	默认动作	含义（发出信号的原因）
SIGHUP		1		Term		终端的挂断或进程死亡
SIGINT		2		Term		来自键盘的中断信号
SIGQUIT		3		Core		来自键盘的离开信号
SIGILL		4		Core		非法指令
SIGABRT		6		Core		来自abort的异常信号
SIGFPE		8		Core		浮点例外
SIGKILL		9		Term		杀死
SIGSEGV		11		Core		段非法错误(内存引用无效)
SIGPIPE		13		Term		管道损坏：向一个没有读进程的管道写数据
SIGALRM		14		Term		来自alarm的计时器到时信号
SIGTERM		15		Term		终止
.......
*/
int32 CThread::kill(int32 sig){
	int32 iRet = 0;
#ifndef WIN32
	if(pthread_equal(m_pthreadID,0) == 0)
		iRet =  pthread_kill(m_pthreadID,sig);
#endif//WIN32
	return iRet;
}

//设定线程绑定状态
/*
@scope = PTHREAD_SCOPE_SYSTEM（绑定)//此线程将与系统中的所有线程进行竞争
		/PTHREAD_SCOPE_PROCESS（非绑定）//此线程将与进程中的其他线程进行竞争。 
*/
int32 CThread::set_scope(int32 scope){
	int32 iRet = 0;
#ifndef WIN32
	if(m_iInit != 0) return m_iInit;
	iRet = pthread_attr_setdetachstate(&m_threadAtrr,scope);
#endif//WIN32
	return iRet;
}

int32 CThread::get_scope(int32*scope){
	int32 iRet = 0;
#ifndef WIN32
	if(m_iInit != 0) return m_iInit;
	iRet = pthread_attr_getscope(&m_threadAtrr,scope);
#endif//WIN32
	return iRet;
}

//设定线程分离状态 @detach_state = PTHREAD_CREATE_DETACHED（分离线程）/PTHREAD_CREATE_JOINABLE（非分离线程）
/*
*分离线程：	如果设置一个线程为分离线程，而这个线程运行又非常快，它很可能在pthread_create函数返回之前就终止了
它终止以后就可能将线程号和系统资源移交给其他的线程使用
这样调用pthread_create的线程就得到了错误的线程号，使用pthread_cond_timewait
*非分离线程：在终止后，必须要有一个线程用 pthread_join 来等待它。否则，不会释放该线程的资源以供新线程使用，而这通常会导致内存泄漏。
因此，如果不希望线程被等待，请将该线程作为分离线程来创建。
*/
int32 CThread::set_detach_state	(int32 detach_state){
	int32 iRet = 0;
#ifndef WIN32
	if(m_iInit != 0) return m_iInit;
	iRet = pthread_attr_setscope(&m_threadAtrr,detach_state);
#endif//WIN32
	return iRet;
}

int32 CThread::get_detach_state(int32*detach_state){
	int32 iRet = 0;
#ifndef WIN32
	if(m_iInit != 0) return m_iInit;
	iRet = pthread_attr_getdetachstate(&m_threadAtrr,detach_state);
#endif//WIN32
	return iRet;
}

// 栈溢出保护区大小
/*
1 溢出保护可能会导致系统资源浪费。如果应用程序创建大量线程，并且已知这些线程永远不会溢出其栈，则可以关闭溢出保护区。通过关闭溢出保护区，可以节省系统资源。
2 线程在栈上分配大型数据结构时，可能需要较大的溢出保护区来检测栈溢出。
*如果 guardsize 为零，则不会为使用 attr 创建的线程提供溢出保护区。如果 guardsize 大于零，则会为每个使用 attr 创建的线程提供大小至少为 guardsize 字节的溢出保护区。缺省情况下，线程具有实现定义的非零溢出保护区。
*/
int32 CThread::set_guard_size(size_t guardsize){
	int32 iRet = 0;
#ifndef WIN32
	if(m_iInit != 0) return m_iInit;
	iRet = pthread_attr_setguardsize(&m_threadAtrr,guardsize);
#endif//WIN32
	return iRet;
}

int32 CThread::get_guard_size(size_t*guardsize){
	int32 iRet = 0;
#ifndef WIN32
	if(m_iInit != 0) return m_iInit;
	iRet = pthread_attr_getguardsize(&m_threadAtrr,guardsize);
#endif//WIN32
	return iRet;
}

//设置调度策略(SCHED_FIFO（先入先出）、SCHED_RR（循环）或 SCHED_OTHER（实现定义的方法）的调度策略属性。)
/*
SCHED_FIFO 
如果调用进程具有有效的用户 ID 0，则争用范围为系统 (PTHREAD_SCOPE_SYSTEM) 的先入先出线程属于实时 (RT) 调度类。如果这些线程未被优先级更高的线程抢占，
则会继续处理该线程，直到该线程放弃或阻塞为止。对于具有进程争用范围 (PTHREAD_SCOPE_PROCESS)) 的线程或其调用进程没有有效用户 ID 0 的线程，请使用 SCHED_FIFO

SCHED_RR 
如果调用进程具有有效的用户 ID 0，则争用范围为系统 (PTHREAD_SCOPE_SYSTEM)) 的循环线程属于实时 (RT) 调度类。如果这些线程未被优先级更高的线程抢占，并且这些线程没有放弃或阻塞，
则在系统确定的时间段内将一直执行这些线程。对于具有进程争用范围 (PTHREAD_SCOPE_PROCESS) 的线程，请使用 SCHED_RR（基于 TS 调度类）。此外，这些线程的调用进程没有有效的用户 ID 0。

SCHED_FIFO 和 SCHED_RR 在 POSIX 标准中是可选的，而且仅用于实时线程。 
*/
int32 CThread::set_sched_policy(int32 policy){
	int32 iRet = 0;
#ifndef WIN32
	if(m_iInit != 0) return m_iInit;
	iRet = pthread_attr_setschedpolicy(&m_threadAtrr,policy);
#endif//WIN32
	return iRet;
}

int32 CThread::get_sched_policy(int32*policy){
	int32 iRet = 0;
#ifndef WIN32
	if(m_iInit != 0) return m_iInit;
	iRet = pthread_attr_getschedpolicy(&m_threadAtrr,policy);
#endif//WIN32
	return iRet;
}

// 继承的调度策略
/*
inherit = PTHREAD_INHERIT_SCHED 表示新建的线程将继承创建者线程中定义的调度策略。将忽略在 pthread_create() 调用中定义的所有调度属性。
inherit = PTHREAD_EXPLICIT_SCHED【缺省值】，则将使用 pthread_create() 调用中的属性。
*/
int32 CThread::set_inherit_sched(int32 inherit){
	int32 iRet = 0;
#ifndef WIN32
	if(m_iInit != 0) return m_iInit;
	iRet = pthread_attr_setinheritsched(&m_threadAtrr,inherit);
#endif//WIN32
	return iRet;
}

int32 CThread::get_inherit_sched(int32*inherit){
	int32 iRet = 0;
#ifndef WIN32
	if(m_iInit != 0) return m_iInit;
	iRet = pthread_attr_getinheritsched(&m_threadAtrr,inherit);
#endif//WIN32
	return iRet;
}

// 调度参数
/*
sched_param param;【仅支持优先级参数】
newprio = 30;
param.sched_priority = newprio;
可以采用两种方式之一来管理 pthreads 优先级：
1： 创建子线程之前，可以设置优先级属性
2：可以更改父线程的优先级，然后再将该优先级改回来
   (获取现有参数 更改优先级 创建子线程 恢复原始优先级)
*/
int32 CThread::set_sched_param(const struct sched_param *param){
	int32 iRet = 0;
#ifndef WIN32
	if(m_iInit != 0) return m_iInit;
	iRet = pthread_attr_setschedparam(&m_threadAtrr,param);
#endif//WIN32
	return iRet;
}

int32 CThread::get_sched_param(struct sched_param *param){
	int32 iRet = 0;
#ifndef WIN32
	if(m_iInit != 0)return m_iInit;
	iRet = pthread_attr_getschedparam(&m_threadAtrr,param);
#endif//WIN32
	return iRet;
}

// 栈大小
/*
一般情况下，不需要为线程分配栈空间。系统会为每个线程的栈分配 1 MB（对于 32 位系统）或 2 MB（对于 64 位系统）的虚拟内存，而不保留任何交换空间。系统将使用 mmap() 的 MAP_NORESERVE 选项来进行分配
PTHREAD_STACK_MIN=默认栈大小
stacksize 属性定义系统分配的栈大小（以字节为单位）。size 不应小于系统定义的最小栈大小。如果 size 为零，则使用缺省大小
PTHREAD_STACK_MIN 是启动线程所需的栈空间量。此栈空间没有考虑执行应用程序代码所需的线程例程要求
例:size = (PTHREAD_STACK_MIN + 0x4000);
指定栈时，还应使用 PTHREAD_CREATE_JOINABLE 创建线程。在该线程的 pthread_join(3C) 调用返回之前，不会释放该栈
*/
int32 CThread::set_stack_size(size_t stacksize){
	int32 iRet = 0;
#ifndef WIN32
	if(m_iInit != 0) return m_iInit;
	iRet = pthread_attr_setstacksize(&m_threadAtrr,stacksize);
#endif//WIN32
	return iRet;
}

int32 CThread::get_stack_size(size_t*stacksize){
	int32 iRet = 0;
#ifndef WIN32
	if(m_iInit != 0) return m_iInit;
	iRet = pthread_attr_getstacksize(&m_threadAtrr,stacksize);
#endif//WIN32
	return iRet;
}

// 栈地址和大小
/*
PTHREAD_STACK_MIN=默认栈大小
void *base;
size_t size;
int ret;
base = (void *) malloc(PTHREAD_STACK_MIN + 0x4000);
ret = pthread_attr_setstack(&tattr, base,PTHREAD_STACK_MIN + 0x4000);
*/
int32 CThread::set_stack(void *stackaddr,size_t stacksize){
	int32 iRet = 0;
#ifndef WIN32
	if(m_iInit != 0) return m_iInit;
	iRet = pthread_attr_setstack(&m_threadAtrr,stackaddr,stacksize);
#endif//WIN32
	return iRet;
}

int32 CThread::get_stack(void **stackaddr,size_t*stacksize){
	int32 iRet = 0;
#ifndef WIN32
	if(m_iInit != 0) return m_iInit;
	iRet = pthread_attr_getstack(&m_threadAtrr,stackaddr,stacksize);
#endif//WIN32
	return iRet;
}