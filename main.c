#include <stdio.h>
#include <process.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#define CLIENTNUM 2


#pragma comment(lib, "ws2_32.lib")

unsigned __stdcall MultiThreadFunc(void* pArguments);




int main()
{

	HANDLE handles[CLIENTNUM] = { NULL, NULL };
	unsigned threadID[CLIENTNUM] = { 0, 0 };

	for (int i = 0; i < CLIENTNUM; ++i)
	{
		handles[i] = (HANDLE)_beginthreadex(NULL, 0, &MultiThreadFunc, (void*)i, 0, &threadID[i]);
		Sleep(1);
	}

	for (int i = 0; i < CLIENTNUM; ++i)
	{

		if (handles[i] != 0)
		{
			WaitForSingleObject(handles[i], INFINITE);
		}

	}

	for (int i = 0; i < CLIENTNUM; ++i)
	{

		if (handles[i] != 0)
		{
			CloseHandle(handles[i]);
		}

	}


	return 0;
}




unsigned __stdcall MultiThreadFunc(void* pArguments)
{
	printf("ThreadNo.%d\n", (int)pArguments);

	WSADATA wsaData[CLIENTNUM];
	SOCKET sock0[CLIENTNUM];
	struct sockaddr_in addr[CLIENTNUM];
	struct sockaddr_in client[CLIENTNUM];
	int len[CLIENTNUM];
	SOCKET sock[CLIENTNUM];
	int sendcheck[CLIENTNUM];
	int recvcheck[CLIENTNUM];


	char buffersend[256];
	char bufferrecv[256];
	int i = 0;

	for (int i = 0; i < CLIENTNUM; i++)
	{
		WSAStartup(MAKEWORD(2, 0), &wsaData[i]);
	}

	for (int i = 0; i < CLIENTNUM; i++)
	{
		sock0[i] = socket(AF_INET, SOCK_STREAM, 0);
	}

	// --- ノンブロッキングモードに設定 ---
	u_long mode[CLIENTNUM];

	for (int i = 0; i < CLIENTNUM; i++)
	{
		mode[i] = 1;
		ioctlsocket(sock0[i], FIONBIO, &mode[i]);
	}


	for (int i = 0; i < CLIENTNUM; i++)
	{
		addr[i].sin_family = AF_INET;
		addr[i].sin_port = htons(5321);
		addr[i].sin_addr.S_un.S_addr = INADDR_ANY;
	}


	for (int i = 0; i < CLIENTNUM; i++)
	{
		bind(sock0[i], (struct sockaddr*)&addr[i], sizeof(addr[i]));

	}

	for (int i = 0; i < CLIENTNUM; i++)
	{
		listen(sock0[i], 5);

	}
	

	for (int i = 0; i < CLIENTNUM; i++)
	{
		len[i] = sizeof(client[i]);

	}
	
	


	i = 0;

	do
	{

		sock[i] = accept(sock0[i], (struct sockaddr*)&client[i], &len[i]);


		if (sock[i] != INVALID_SOCKET)
		{
			break;
		}



		if (i == 1)
		{
			i = 0;
		}

		++i;

	} while (1);



	printf("*************************************************\n");
	strcpy(buffersend, "FROM SERVER");



	i = 0;


	do
	{

		sendcheck[i] = send(sock[i], buffersend, strlen(buffersend), 0);


		if (sendcheck[i] != SOCKET_ERROR)
		{
			break;
		}

		if (i == 1)
		{
			i = 0;
		}


		++i;




	} while (1);

	i = 0;


	do
	{

		recvcheck[i] = recv(sock[i], bufferrecv, strlen(bufferrecv), 0);



		if (recvcheck[i] != SOCKET_ERROR)
		{
			break;
		}

		if (i == 1)
		{
			i = 0;
		}

		++i;

	} while (1);



	closesocket(sock);
	WSACleanup();



	return 0;
}
