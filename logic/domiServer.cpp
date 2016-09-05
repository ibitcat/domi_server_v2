#include <signal.h>
#include <iostream>
#include "domiServer.h"
#include "../common/log/log.h"

// 处理消息
static uint64 STnowTime = getSecond();
static uint32 STnum = 0;
static uint32 STCount = 0;
static int g_exiting = 0;
static int g_stop = 0;

static void	onBreak(int){
#ifdef WIN32
	::AllocConsole();
#endif// WIN32
	g_exiting++;
	CDomiServer::GetSingleton().denyServices();
}

#ifdef WIN32
HANDLE	CDomiServer::m_hServerEvent = nullptr;
#endif

CDomiServer::CDomiServer(){
}

CDomiServer::~CDomiServer(){
}

void CDomiServer::installBreakHandlers(){
#ifdef WIN32
	::signal(SIGBREAK, onBreak);
#else
	::signal(SIGSTOP, onBreak);//停止进程
	::signal(SIGTSTP, onBreak);//终端上发出的停止信号
	::signal(SIGKILL, onBreak);//杀死进程信号
	::signal(SIGHUP, onBreak);//在控制终端上是挂起信号, 或者控制进程结束
#endif
	::signal(SIGTERM, onBreak);	//使用kill指令时
	::signal(SIGINT, onBreak);	//Ctrl+C的时候进程收到信号，触发OnBreak函数
}

bool CDomiServer::loadConfig(){
	return true;
}

void CDomiServer::showToConsole(const char* pFormat, ...){
	if (!pFormat || !*pFormat)
		return;

	char szStrint[2048] = { 0 };
	va_list	argptr;
	va_start(argptr, pFormat);
	dVsprintf(szStrint, sizeof(szStrint), pFormat, argptr);
	va_end(argptr);

	CConsoleColor clColor(_BIT32(CConsoleColor::color_green) | _BIT32(CConsoleColor::color_intensity));
	dPrintf("%s\n", szStrint);
}

void CDomiServer::setServicesTitle(const char* pTitle, ...){
#ifdef WIN32 // 设置控制台标题
	if (!pTitle && m_szTitle[0])
		return;

	char szTitle[256] = { 0 };
	if (pTitle) {
		va_list	argptr;
		va_start(argptr, pTitle);
		dVsprintf(szTitle, sizeof(szTitle), pTitle, argptr);
		va_end(argptr);
	}

	if (!m_tmStartTime)
		m_tmStartTime = getTime();

	tm	tmTime;
	getLocalTime(&tmTime, &m_tmStartTime);
	dSprintf(m_szTitle, sizeof(m_szTitle), "%s StartTime: %02d-%02d %02d:%02d:%02d", szTitle, tmTime.tm_mon + 1, tmTime.tm_mday, tmTime.tm_hour, tmTime.tm_min, tmTime.tm_sec);
	::SetConsoleTitle(m_szTitle);
#endif
}

void CDomiServer::showServerInfo(){
	showToConsole("***************************************************************");
	showToConsole("* Game Server Application Ver %d.%d", 0, 0);
	showToConsole("***************************************************************");
	setServicesTitle("Game:<%.4d>", 1);
}

bool CDomiServer::initialize(uint16 uServerID) {
	showServerInfo();

#ifdef WIN32
	char szEvent[256] = { 0 };	/*唯一事件*/
	dSprintf(szEvent, sizeof(szEvent), "%s_%d", "DomiServer", uServerID);
	// 如果可以打开指定事件名的事件，说明已经存在该进程
	m_hServerEvent = ::OpenEvent(EVENT_ALL_ACCESS, FALSE, szEvent);
	if (m_hServerEvent)
		return false;
	m_hServerEvent = ::CreateEvent(nullptr, FALSE, FALSE, szEvent);
#endif

	char logprev[256] = { 0 };
	dSprintf(logprev, sizeof(logprev), "%s_%u", "test_log_", uServerID);
	CLog::initialize(NULL, logprev);// 日志初始化

	// 这里必须在CLog::initilize之后
	// 守护进程 TODO
	if (m_bInitFlag) return true;

	::srand((uint32)getTime());		// 设置随机种子
	m_uServerID = uServerID;		// 设置服务器id

	if (!loadConfig()) {// load 服务器配置
		CLog::error("配置文件装载失败!...");
		return false;
	}

	installBreakHandlers();
	m_tmStartTime = getTime();
	m_bInitFlag = true;
	return true;
}

bool CDomiServer::startServices(){
	CLog::print("hahaha,测试汉字打印");
	CLog::error("test%s,error","错误日志");
	CLog::warn("test%s,warn","警告日志");
	if (!m_el.StartServer(7777)){
		return false;
	}
	return true;
}

void CDomiServer::denyServices(){
	m_el.StopServer();
}

void CDomiServer::stopServices(){
	CLog::info("server shutdown!...");
	// 各种底层的shutdown
	CLog::shutdown();
}
