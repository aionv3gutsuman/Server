#include <stdio.h>
#include <process.h>
#include <winsock2.h>

#define CLIENTNUM 2


#pragma comment(lib, "ws2_32.lib")

unsigned __stdcall MultiThreadFunc(void* pArguments);




int main()
{

	HANDLE handles[CLIENTNUM] = { NULL,NULL };
	unsigned threadID[CLIENTNUM] = { 0,0 };

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

	WSADATA wsaData;
	SOCKET sock0;
	struct sockaddr_in addr;
	struct sockaddr_in client;
	int len;
	SOCKET sock;
	int sendcheck;
	int recvcheck;


	char buffer[256];

	WSAStartup(MAKEWORD(2, 0), &wsaData);

	sock0 = socket(AF_INET, SOCK_STREAM, 0);

	addr.sin_family = AF_INET;
	addr.sin_port = htons(5321);
	addr.sin_addr.S_un.S_addr = INADDR_ANY;

	bind(sock0, (struct sockaddr*)&addr, sizeof(addr));

	listen(sock0, 5);
	len = sizeof(client);
	while ((sock = accept(sock0, (struct sockaddr*)&client, &len)) == INVALID_SOCKET)
	{



		closesocket(sock);
	}

	strcpy(buffer, "FROM SERVER");

	while (1)
	{
		sendcheck = send(sock, buffer, strlen(buffer), 0);
		if (sendcheck != SOCKET_ERROR)
		{
			printf("SUCCESS SEND\n");
			break;
		}
		closesocket(sock);
	}


	while (1)
	{
		recvcheck = recv(sock, buffer, strlen(buffer), 0);
		if (sendcheck != SOCKET_ERROR)
		{
			printf("%s\n", buffer);
			break;
		}
		closesocket(sock);
	}


	WSACleanup();



	return 0;
}