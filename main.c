#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>   // 必要なら「必ず」winsock2.h の後
#include <stdio.h>
#include <process.h>

#pragma comment(lib, "ws2_32.lib")

#define CLIENTNUM 2

unsigned __stdcall MultiThreadFunc(void* pArguments);

int main()
{
	HANDLE handles[CLIENTNUM];
	unsigned threadID[CLIENTNUM];

	for (int i = 0; i < CLIENTNUM; ++i)
	{
		handles[i] = (HANDLE)_beginthreadex(NULL, 0, &MultiThreadFunc, (void*)i, 0, &threadID[i]);
		Sleep(1);
	}

	for (int i = 0; i < CLIENTNUM; ++i)
	{
		WaitForSingleObject(handles[i], INFINITE);
		CloseHandle(handles[i]);
	}

	return 0;
}

unsigned __stdcall MultiThreadFunc(void* pArguments)
{
	printf("ThreadNo.%d\n", (int)pArguments);

	WSADATA wsaData[CLIENTNUM];
	SOCKET sock0[CLIENTNUM];
	struct sockaddr_in addr[CLIENTNUM];
	SOCKET clientSock = INVALID_SOCKET;
	struct sockaddr_in clientAddr;
	int clientLen = sizeof(clientAddr);

	char buffersend[256] = { 0 };
	char bufferrecv[256] = { 0 };

	for (int i = 0; i < CLIENTNUM; i++)
	{
		WSAStartup(MAKEWORD(2, 0), &wsaData[i]);
	}

	

	// --- 2つのリッスンソケットを作成 ---
	for (int i = 0; i < CLIENTNUM; i++)
	{
		sock0[i] = socket(AF_INET, SOCK_STREAM, 0);

		u_long mode = 1;
		ioctlsocket(sock0[i], FIONBIO, &mode);

		addr[i].sin_family = AF_INET;
		addr[i].sin_addr.S_un.S_addr = INADDR_ANY;
		addr[i].sin_port = htons(i == 0 ? 5000 : 6000);

		bind(sock0[i], (struct sockaddr*)&addr[i], sizeof(addr[i]));
		listen(sock0[i], 5);
	}

	// --- select で accept 待ち ---
	fd_set rfds;
	printf("Waiting for connection...\n");

	while (1)
	{
		FD_ZERO(&rfds);
		FD_SET(sock0[0], &rfds);
		FD_SET(sock0[1], &rfds);

		int ret = select(0, &rfds, NULL, NULL, NULL);

		if (ret > 0)
		{
			for (int i = 0; i < CLIENTNUM; i++)
			{
				if (FD_ISSET(sock0[i], &rfds))
				{
					clientSock = accept(sock0[i], (struct sockaddr*)&clientAddr, &clientLen);
					if (clientSock != INVALID_SOCKET)
					{
						printf("Accepted on port %d\n", ntohs(addr[i].sin_port));
						Sleep(20000);
						goto ACCEPTED;
					}
				}
			}
		}
	}

ACCEPTED:

	// --- 送信 ---
	strcpy(buffersend, "FROM SERVER");
	send(clientSock, buffersend, strlen(buffersend), 0);

	// --- 受信 ---
	int recvcheck = recv(clientSock, bufferrecv, sizeof(bufferrecv), 0);
	if (recvcheck > 0)
	{
		printf("RECV: %s\n", bufferrecv);
	}

	closesocket(clientSock);
	closesocket(sock0[0]);
	closesocket(sock0[1]);
	WSACleanup();

	return 0;
}
