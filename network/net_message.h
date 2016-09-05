// msgpack 直接能给业务层使用的
#pragma once
#include "../common/basic/basicTypes.h"

/*
协议数据使用google protobuf
message NetMessage{
	uint16 uProtocol = 1;	// 协议号
	uint8 type = 2;			// 消息包类型，1=请求包（出去的包），2 = 返回包（进来的包）
	optional bytes content = 3;	// 消息字节
}
*/

#pragma pack(push,1) //禁用字节对齐
struct _stNetMessage 
{
	uint32 m_op;		//协议号
	uint32 m_mask;		//mask
	uint16 m_size;		//数据大小
	char m_buffer[1024];//数据内容
	
	_stNetMessage()
		:m_op(0),
		m_mask(0), 
		m_size(0){
		//m_proto.Clear();
		memset(m_buffer, 0, sizeof(m_buffer));
	}

	inline void	reset(uint32 apply_key, char* buffer, uint16 size){
		m_op = apply_key;
		//m_proto.SerializeToArray(buffer, size);		
		memcpy(m_buffer,buffer,size);
		m_size = size;
	}
};
#pragma pack(pop)
