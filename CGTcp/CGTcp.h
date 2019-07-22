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
		SOCKET m_sock;			//�׽���
		sockaddr_in m_stAddr;	//��������Ϣ
		bool m_bExit;			//�����߳��˳�
		int m_nBufferSize;		//��������С

	public:
		CGTcp();
		virtual ~CGTcp();

		static void RecvThread(CGTcp* pThis);				//���������߳�

		virtual void OnRecv(void* pData, int nRecvSize) = 0;//���������¼�֪ͨ

		bool Connect(const char* szIP,int nPort);			//���ӷ�����
		bool Close();										//�ر�����
		bool SendBuffer(void* pData,int nSize);				//��������

		inline void SetBufferSize(int nSize)				//���û�������С
		{
			m_nBufferSize = nSize;
		}			
	};
}
