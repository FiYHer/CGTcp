#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#pragma comment(lib,"WS2_32.lib")

#include <thread>

namespace cgtcp
{
	class CGTcp
	{
	private:
		SOCKET m_sock;			//套接字
		sockaddr_in m_stAddr;	//服务器信息
		bool m_bExit;			//接收线程退出
		int m_nBufferSize;		//缓冲区大小

	public:
		CGTcp();
		virtual ~CGTcp();

		static void RecvThread(CGTcp* pThis);				//接收数据线程

		virtual void OnRecv(void* pData, int nRecvSize) = 0;//接收数据事件通知

		bool Connect(const char* szIP,int nPort);			//连接服务器
		bool Close();										//关闭连接
		bool SendBuffer(void* pData,int nSize);				//发送数据

		inline void SetBufferSize(int nSize)				//设置缓冲区大小
		{
			m_nBufferSize = nSize;
		}			
	};
}
