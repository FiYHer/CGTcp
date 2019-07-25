#include "CGTcp.h"

cgtcp::CGTcp::CGTcp()
{
	m_sock = INVALID_SOCKET;

	m_bRecvExit = true;
	m_nRecvBufferSize = 4 * 1024;
	m_nRecvTick = 15;

	m_bSendExit = true;

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
	std::unique_ptr<char> pUniquePrt(new char[pThis->m_nRecvBufferSize]);
	char* szData = pUniquePrt.get();
	memset(szData, 0, pThis->m_nRecvBufferSize);
	WSABUF stBuffer;
	stBuffer.buf = szData;
	stBuffer.len = pThis->m_nRecvBufferSize;
	DWORD dwRecvByte = 0, dwFlag = 0;
	int nError = 0;
	while (pThis->m_bRecvExit == false && pThis->m_sock != INVALID_SOCKET)
	{
		nError = dwRecvByte = 0;
		WSARecv(pThis->m_sock, &stBuffer, 1, &dwRecvByte, &dwFlag, 0, 0);
		nError = WSAGetLastError();
		if (WSAGetLastError() == WSAECONNABORTED)	//连接关闭
		{
			pThis->Close();
			pThis->OnClose();
			break;
		}
		else if (dwRecvByte)						//数据接收
			pThis->OnRecv((void*)szData, dwRecvByte);
		Sleep(15);									//没有间隔会消耗大量CPU
	}
}

void cgtcp::CGTcp::SendThread(CGTcp* pThis, int nTick, void* pBuffer, int nSendSize)
{
	std::unique_ptr<char> pUniquePrt(new char[nSendSize]);
	char* szData = pUniquePrt.get();
	memcpy(szData, pBuffer, nSendSize);
	while (pThis->m_bSendExit == false)
	{
		if (pThis->SendBuffer((void*)szData, nSendSize) == false)
		{
			pThis->Close();
			pThis->OnClose();
			break;
		}
		pThis->OnSend((void*)szData, nSendSize);
		Sleep(nTick);
	}
	pThis->m_bSendExit = true;
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

	//将套接字修改为非阻塞模式
	u_long ulFlag = 1;
	ioctlsocket(m_sock, FIONBIO, &ulFlag);

	m_bRecvExit = false;
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
		m_bRecvExit = true;
		m_bSendExit = true;
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

	DWORD dwSendByte = 0, dwFlag = 0;
	WSABUF stBuffer;
	stBuffer.buf = (char*)pData;
	stBuffer.len = nSize;
	WSASend(m_sock, &stBuffer, 1, &dwSendByte, dwFlag, 0, 0);
	if (WSAGetLastError() == WSAECONNABORTED)
		return false;
	return true;
}

bool cgtcp::CGTcp::SatrtSendBufferFromTick(int nTick, void* pBuffer, int nSendSize)
{
	if (m_bSendExit == false)
		return false;

	m_bSendExit = false;
	std::thread cThread(SendThread, this, nTick, pBuffer, nSendSize);
	cThread.detach();
	return true;
}
