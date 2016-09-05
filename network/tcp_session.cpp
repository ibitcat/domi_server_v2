/******************************************************************** 
创建时间:        2015/06/28 14:03
文件名称:        tcp_context.cpp
文件作者:        Domi
*********************************************************************/

#include <functional>
#include "tcp_session.h"
#include "tcp_server.h"
#include "../common/log/log.h"
#include "../common/basic/memoryFunctions.h"

CTcpSession::CTcpSession()
	:m_id(-1){
	initialize();
}

CTcpSession::~CTcpSession(){
	m_id = 0;
	m_mask = 0;
	if (m_fd>0){
#ifdef WIN32
		closesocket(m_fd);
#else
		close(m_fd);
#endif
		m_fd = 0;
	}
}

// 初始化
void CTcpSession::initialize(){
	m_mask = 0;
	m_fd = 0;
	m_inbufLen = 0;
	m_readBegin = 0;
}

bool CTcpSession::send(const char* pBuffer,int32 nSize){

	uint32 msgLen = sizeof(PacketHead) + nSize;
	PacketHead* head = (PacketHead*)(m_income);
	head->uPacketSize = msgLen;
	head->uHeadFlag = 10;

	dMemcpy(m_income + sizeof(PacketHead), sizeof(m_income), pBuffer, nSize);
	return true;
}

void CTcpSession::disconnect(){
}

// 远程地址
ulong CTcpSession::remote_address(){
	return 0;
}

// 远程地址的ip
const char*	CTcpSession::remote_ip(){
	return "test";
}
