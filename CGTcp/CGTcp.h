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
		SOCKET m_sock;			//�׽���
		sockaddr_in m_stAddr;	//��������Ϣ

		bool m_bRecvExit;		//�����߳��˳�
		int m_nRecvBufferSize;	//���ջ�������С
		int m_nRecvTick;		//�������ݼ��

		bool m_bSendExit;		//�����߳��˳�

	public:
		CGTcp();
		virtual ~CGTcp();

		static void RecvThread(CGTcp* pThis);				//���������߳�
		static void SendThread(CGTcp* pThis,
			int nTick, void* pBuffer, int nSendSize);		//���������߳�

		virtual void OnRecv(void* pData, int nRecvSize) = 0;//���������¼�֪ͨ
		virtual void OnSend(void* pData, int nSendSize) {};	//��������ʱ��֪ͨ
		virtual void OnClose() {};							//���ӹر�ʱ��

		bool Connect(const char* szIP,int nPort);			//���ӷ�����
		bool Close();										//�ر�����
		bool SendBuffer(void* pData,int nSize);				//��������
		bool SatrtSendBufferFromTick(
			int nTick,void* pBuffer,int nSendSize);			//��ʱ��������
		
		inline void StopSendBuffer()//ֹͣ��ʱ����
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
		inline bool IsConnect()//�ж��Ƿ����ӷ�����
		{
			if (m_sock != INVALID_SOCKET)
				return true;
			return false;
		}
		inline bool IsRunRecv()//�жϽ����߳��Ƿ�����
		{
			return !m_bRecvExit;
		}
		inline bool IsRunSend()//�жϷ����߳��Ƿ�����
		{
			return !m_bSendExit;
		}
	};
}
