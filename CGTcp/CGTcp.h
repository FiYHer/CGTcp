#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#pragma comment(lib,"WS2_32.lib")

#include <thread>

namespace cgtcp
{
	class CGTcp
	{
	public:
		SOCKET m_sock;			//套接字
		sockaddr_in m_stAddr;	//服务器信息

		bool m_bRecvExit;		//接收线程退出
		int m_nRecvBufferSize;	//接收缓冲区大小
		int m_nRecvTick;		//接收数据间隔

		bool m_bSendExit;		//发送线程退出

	public:
		CGTcp();
		virtual ~CGTcp();

		static void RecvThread(CGTcp* pThis);				//接收数据线程
		static void SendThread(CGTcp* pThis,
			int nTick, void* pBuffer, int nSendSize);		//发送数据线程

		virtual void OnRecv(void* pData, int nRecvSize) = 0;//接收数据事件通知
		virtual void OnSend(void* pData, int nSendSize) {};	//发送数据时间通知
		virtual void OnClose() {};							//连接关闭时间

		bool Connect(const char* szIP,int nPort);			//连接服务器
		bool Close();										//关闭连接
		bool SendBuffer(void* pData,int nSize);				//发送数据
		bool SatrtSendBufferFromTick(
			int nTick,void* pBuffer,int nSendSize);			//定时发送数据
		
		inline void StopSendBuffer()//停止定时发送
		{
			m_bSendExit = true;
		}

		inline void SetRecvBufferSize(int nSize)
		{
			m_nRecvBufferSize = nSize;
		}
		inline void SetRecvTick(int nTick)
		{
			m_nRecvTick = nTick;
		}
		inline bool IsConnect()//判断是否连接服务器
		{
			if (m_sock != INVALID_SOCKET)
				return true;
			return false;
		}
		inline bool IsRunRecv()//判断接收线程是否运行
		{
			return !m_bRecvExit;
		}
		inline bool IsRunSend()//判断发送线程是否运行
		{
			return !m_bSendExit;
		}
	};
}
