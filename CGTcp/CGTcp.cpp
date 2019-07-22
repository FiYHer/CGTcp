#include "CGTcp.h"

cgtcp::CGTcp::CGTcp()
{
	m_sock = INVALID_SOCKET;
	m_bExit = true;
	m_nBufferSize = 4 * 1024;

	memset(&m_stAddr, 0, sizeof(sockaddr_in));
	m_stAddr.sin_family = AF_INET;

	WSADATA stData;
	if (WSAStartup(MAKEWORD(2, 2), &stData) != 0)
		exit(0);
}

cgtcp::CGTcp::~CGTcp()
{
	this->Close();
	WSACleanup();
}

void cgtcp::CGTcp::RecvThread(CGTcp* pThis)
{
	std::unique_ptr<char> pUniquePrt(new char[pThis->m_nBufferSize]);
	char* szData = pUniquePrt.get();
	memset(szData, 0, pThis->m_nBufferSize);
	WSABUF stBuffer;
	stBuffer.buf = szData;
	stBuffer.len = pThis->m_nBufferSize;
	DWORD dwRecvByte = 0;
	DWORD dwFlag = 0;
	while (pThis->m_bExit == false && pThis->m_sock != INVALID_SOCKET)
	{
		WSARecv(pThis->m_sock, &stBuffer, 1, &dwRecvByte, &dwFlag, 0, 0);
		//通知
		pThis->OnRecv((void*)szData, dwRecvByte);
		//如果为0就代表服务器将我们的连接关闭了
		if (dwRecvByte == 0)
			pThis->Close();
	}
}

bool cgtcp::CGTcp::Connect(const char* szIP, int nPort)
{
	if (!szIP)
		return false;

	if (m_sock != INVALID_SOCKET)
		return false;

	m_sock = WSASocketA(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, 0);
	if (m_sock == INVALID_SOCKET)
		return false;

	m_stAddr.sin_addr.S_un.S_addr = inet_addr(szIP);
	m_stAddr.sin_port = htons(nPort);
	if (WSAConnect(m_sock, (sockaddr*)&m_stAddr,
		sizeof(sockaddr), 0, 0, 0, 0) == SOCKET_ERROR)
	{
		closesocket(m_sock);
		m_sock = INVALID_SOCKET;
		return false;
	}

	m_bExit = false;
	std::thread cThread(RecvThread, this);
	cThread.detach();

	return true;
}

bool cgtcp::CGTcp::Close()
{
	if (m_sock != INVALID_SOCKET)
	{
		closesocket(m_sock);
		m_sock = INVALID_SOCKET;
		m_bExit = true;
		return true;
	}
	return false;
}

bool cgtcp::CGTcp::SendBuffer(void* pData, int nSize)
{
	if (!pData || nSize <= 0)
		return false;

	if (m_sock == INVALID_SOCKET)
		return false;

	DWORD dwSendByte  = 0;
	DWORD dwFlag = 0;
	WSABUF stBuffer;
	stBuffer.buf = static_cast<char*>(pData);
	stBuffer.len = nSize;
	WSASend(m_sock, &stBuffer, 1, &dwSendByte, dwFlag, 0, 0);
	return static_cast<bool>(dwSendByte);
}
