/*
* TCP/IP Socket Programming - 네트워크 프로그래밍과 소켓의 이해
* 파일명: hello_client_win.c
* 파일 버전: 0.1
* 작성자: Sevenshards
* 작성 일자: 2023-12-15
* 이전 버전 작성 일자:
* 버전 내용: 간단한 소켓 프로그래밍 예제 - Client, Windows, MBCS기반
* 이전 버전 내용:
*/

#pragma comment(lib, "ws2_32.lib")

#include <stdio.h>
#include <stdlib.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

void ErrorHandling(const char* message);

int main(int argc, char* argv[])
{
	WSADATA wsaData;
	SOCKET hSocket;
	SOCKADDR_IN servAddr;

	char message[30];
	int strLen;
	if (argc != 3)
	{
		printf("Usage: %s <IP> <Port>\n", argv[0]);
		exit(1);
	}

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		ErrorHandling("WSAStartup() error!");

	hSocket = socket(PF_INET, SOCK_STREAM, 0);
	if (hSocket == INVALID_SOCKET)
		ErrorHandling("socket() error!");

	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	// servAddr.sin_addr.s_addr = inet_addr(argv[1]);
	// 현재 iner_addr은 deprecated되어 곧 쓰지 않을 수 있음
	// 따라서 inet_pton() 또는 InetPton()을 사용할 것을 권장 -> header파일로 WS2tcpip.h 추가 필요
	// pton == pc to network / ntop == network to pc
	// #define _WINSOCK_DEPRECATED_NO_WARNINGS를 사용해도 되지만 여기서는 권장하는 방법을 사용해보려고 함
	// 반환값은 정수형
	inet_pton(AF_INET, argv[1], &servAddr.sin_addr);
	servAddr.sin_port = htons(atoi(argv[2]));

	// 리눅스에서의 connect
	if (connect(hSocket, (SOCKADDR*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR)
		ErrorHandling("connect() error!");

	// 리눅스에서의 read, 여기서는 recv(receive)
	strLen = recv(hSocket, message, sizeof(message) - 1, 0);
	if (strLen == -1)
		ErrorHandling("recv() error!");

	printf("Message from server: %s\n", message);

	closesocket(hSocket);
	WSACleanup();
	return 0;
}


void ErrorHandling(const char* message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}