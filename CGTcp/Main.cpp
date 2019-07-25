
#include <stdio.h>
#include "CGTcp.h"
using namespace cgtcp;

class MyClass : public CGTcp
{
public:
	MyClass()
	{

	}
	~MyClass()
	{

	}
	virtual void OnRecv(void* pData, int nRecvSize)
	{
		static int nIndex = 0;
		if (nRecvSize == 0)
		{
			printf("�������ر�����\n");
		}
		else
		{
			printf("Index : %d  Data : %s  Size : %d\n",
				nIndex++, (char*)pData, nRecvSize);
			SendBuffer(pData, nRecvSize);
		}
	}
	virtual void OnClose()
	{
		printf("�Ͽ�����\n");
	}
};

int main(int argc, char* argv[])
{
	MyClass Test;
	if (Test.Connect("127.0.0.1", 6666))
	{
		printf("���ӳɹ�\n");
		char szBuffer[20] = "Buffer is Test";
		Test.SatrtSendBufferFromTick(200, (void*)szBuffer, strlen(szBuffer));
		getchar();
		Test.StopSendBuffer();
		getchar();
	}
	return 0;
}