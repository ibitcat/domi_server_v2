// server

#include "../common/singleton/singleton.h"
#include "../common/basic/stringFunctions.h"
#include "../network/tcp_server.h"

class CDomiServer : public Singleton<CDomiServer>
{
public:
	CDomiServer();
	~CDomiServer();

public:
	void installBreakHandlers();
	bool loadConfig();
	void showServerInfo();
	void showToConsole(const char* pFormat, ...);
	void setServicesTitle(const char* pTitle, ...);
	bool initialize(uint16 uServerID);	// 初始化server
	bool startServices();				// 启动server
	void denyServices();				// 拒绝服务
	void stopServices();				// 关闭server
	
public:
	CTcpServer	m_el;					// eventloop
	bool		m_bInitFlag;			// server是否已经初始化完成
	uint32		m_uLoadFlag;			// 读取配置完成标示
	uint64		m_tmStartTime;			// 启动时间
	char		m_szTitle[256];			// 标题
	uint8		m_uServerType;			// 服务器类型
	uint16		m_uServerID;			// 服务器id
#ifdef WIN32
	static HANDLE m_hServerEvent;		// 全局事件句柄，windows下作唯一进程标示
#endif
};
