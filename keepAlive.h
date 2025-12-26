#include <windows.h>


void keepAlive()
{

	int count = 0;
	int timeout = 0;
	while (timeout <= 5)
	{

		Sleep(100);
		++count;
		timeout = count * 0.1;
	}


}