/******************************************************************** 
创建时间:        2015/07/01 9:56
文件名称:        CMessageQueue.h
文件作者:        Domi
功能说明:        消息队列  
其他说明:        采用双队列切换。
*********************************************************************/

#pragma once
#include "../common/pool/queue_pool.h"
#include "net_message.h"

class CMessageQueue
{
public:
	enum
	{
		message_pop	,//取出队列
		message_push,//加入队列
		message_max
	};
	typedef CQueuePool<_stNetMessage> msg_queue;

public:
	CMessageQueue(){
		reset();
		queue_max(60000);	// 每个队列最多有6w个消息
	}
	~CMessageQueue(){}

public:
	inline void			queue_max(uint32 _max)	{	m_queue_max = _max;				}
	inline msg_queue*	get_pop	()				{	return m_message[message_pop];	}
	inline msg_queue*	get_push()				{	return m_message[message_push];	}

public:
	inline void	reset();					// 复位(加锁,避免多线程正在写)
	inline void	swap_queue();				// 交换消息队列(加锁,避免多线程正在写)
	inline _stNetMessage* front();			// 取出消息头(单线程不需要加锁)
	inline void	pop();						// 取出消息(单线程不需要加锁)
	inline bool	push(uint32 _apply_key,char*_buffer, uint16 _size);	// 写入消息(多线程需要加锁)

protected:
	uint32		m_queue_max;				//队列最大数量
	msg_queue	m_instance[message_max];	//消息实例
	msg_queue*	m_message[message_max];		//消息指针
};

//-----------------------------------------------------------------------
// 重置消息队列
inline void	CMessageQueue::reset()
{
	try
	{
		CCritLocker clLock_pop(m_instance[message_pop].get_mutex());
		CCritLocker clLock_push(m_instance[message_push].get_mutex());

		for (int i = 0;i < message_max;i++)
		{
			m_message[i] = &m_instance[i];
			m_instance[i].clear();
		}
	}
	catch (...)
	{
	}
}

// 交换消息队列
inline void	CMessageQueue::swap_queue()
{
	try
	{
		//切换时需要对两个都加锁
		CCritLocker clLock_pop(m_instance[message_pop].get_mutex());
		CCritLocker clLock_push(m_instance[message_push].get_mutex());

		msg_queue* _temp			= m_message[message_pop	];
		m_message[message_pop	]	= m_message[message_push];
		m_message[message_push	]	= _temp;
	}
	catch (...)
	{
	}
}

inline _stNetMessage* CMessageQueue::front()
{
	try
	{
		if(m_message[message_pop])
			return m_message[message_pop]->front();
	}
	catch (...)
	{
	}

	return nullptr;
}

inline void	CMessageQueue::pop()
{
	try
	{
		if(m_message[message_pop])
			return m_message[message_pop]->pop();
	}
	catch (...)
	{
	}
}

inline bool	CMessageQueue::push(uint32 _apply_key,char* _buffer,uint16 _size)
{
	try
	{
		if(!m_message[message_push])
			return false;

		//增加锁
		CCritLocker clLock(m_message[message_push]->get_mutex());
		if(m_queue_max && m_message[message_push]->size() > m_queue_max)
			return false;

		_stNetMessage* pMsg = m_message[message_push]->push();
		if(!pMsg)
			return false;

		//pMsg->reset(_apply_key, pContext, _buffer, _size);

		return true;
	}
	catch (...)
	{
	}
	return false;
}