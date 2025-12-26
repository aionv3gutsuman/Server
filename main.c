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
	SOCKET clientSock[CLIENTNUM];
	struct sockaddr_in clientAddr;
	int clientLen = sizeof(clientAddr);
	int recvcheck[CLIENTNUM];

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
					clientSock[i] = accept(sock0[i], (struct sockaddr*)&clientAddr, &clientLen);
					if (clientSock[i] != INVALID_SOCKET)
					{

						if (i == 0)
						{
							printf("ポート5000\n");
						}
						if (i == 1)
						{
							printf("ポート6000\n");
						}
						//Sleep(20000);
						goto ACCEPTED;
					}
				}
			}
		}
	}

ACCEPTED:

// --- 送信 ---
strcpy(buffersend, "FROM SERVER");

while (1)
{
    for (int i = 0; i < CLIENTNUM; i++)
    {
        if (clientSock[i] == INVALID_SOCKET)
            continue;

        fd_set wfds;
        FD_ZERO(&wfds);
        FD_SET(clientSock[i], &wfds);

        TIMEVAL tv;
        tv.tv_sec = 5;
        tv.tv_usec = 0;

        int ret = select(0, NULL, &wfds, NULL, &tv);

        if (ret > 0 && FD_ISSET(clientSock[i], &wfds))
        {
            int sret = send(clientSock[i], buffersend, strlen(buffersend), 0);

            if (sret > 0)
            {
                printf("send成功 (client %d)\n", i);
                goto SEND;
            }
            else
            {
                int err = WSAGetLastError();
                if (err != WSAEWOULDBLOCK)
                {
                    printf("send error: %d\n", err);
                }
            }
        }
    }

    Sleep(1); // CPU暴走防止
}

SEND:
	


	while (1)
	{
		for (int i = 0; i < CLIENTNUM; i++)
		{

			recvcheck[i] = recv(clientSock[i], bufferrecv, sizeof(bufferrecv), 0);
			if (recvcheck[i] > 0)
			{
				printf("RECV: %s\n", bufferrecv);
				goto RECV;
			}
		}
	}


RECV:


	for (int i = 0; i < CLIENTNUM; i++)
	{
		if (clientSock[i] != INVALID_SOCKET)
			closesocket(clientSock[i]);
	}
	closesocket(sock0[0]);
	closesocket(sock0[1]);
	WSACleanup();

	return 0;
}
