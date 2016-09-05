#pragma once

#ifdef WIN32
#include <winsock2.h>
#include <mswsock.h>
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "Mswsock.lib")
#pragma comment (lib,"wsock32.lib")
#else
#include <netinet/in.h>//包含了<sys/socket.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/types.h>//系统类型定义
#include <arpa/inet.h>
#include <fcntl.h>//阻塞模式头文件
#include <netdb.h>
#include <unistd.h>//close函数
#endif
#include <errno.h>	//包含此文件就会设置errno变量【int geterror()】

#include "../common/basic/basicTypes.h"

#define MaxBuffLen 1024*10			// 10k
#define MaxBuffLenLimit 1024*20		// 20k

// 协议包头
struct PacketHead
{
	uint16	uHeadFlag;		// 包头标示
	uint16	uPacketSize;	// 包大小
	PacketHead() {memset( this, 0, sizeof( *this ) );	}
};

class CTcpServer;
class CTcpSession		//状态有【链接中|已链接|断开中】
{
public:
	CTcpSession();
	~CTcpSession();

public:
	inline int getPendingLen()	{ return m_inbufLen - m_readBegin; }// 剩余未处理的字节数
	inline int getFreeLen()		{ return MaxBuffLen - m_inbufLen; }	// 可以append的长度

	void initialize();	// 初始化
	bool send(const char* pBuffer,int32 nSize);
	void disconnect();			// 断开连接
	ulong remote_address();		// 远程地址
	const char*	remote_ip();	// 远程地址的ip

public:
	// char* p = new char[100]; delete []p;
	CTcpServer*	m_tcpServer;	// 所属的server
	int m_fd;					// socket fd
	int m_id;					// session id(给上层用的)
	int m_mask;					// 标示
	char* m_income;	
	char* m_outcome;
	int m_inbufLen;				// inbuff 结束位置
	int m_readBegin;			// 下一次开始读取的位置
};
