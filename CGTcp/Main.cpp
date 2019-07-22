
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
			printf("服务器关闭连接\n");
		}
		else
		{
			printf("Index : %d  Data : %s  Size : %d\n",
				nIndex++, (char*)pData, nRecvSize);
			//SendBuffer(pData, nRecvSize);
		}
	}
};

int main(int argc, char* argv[])
{
	MyClass Test;
	Test.Connect("127.0.0.1", 6666);
	getchar();
	Test.Close();
	getchar();
	Test.Connect("127.0.0.1", 6666);
	getchar();
	return 0;
}