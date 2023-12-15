/*
* TCP/IP Socket Programming - 네트워크 프로그래밍과 소켓의 이해
* 파일명: hello_server_win.c
* 파일 버전: 0.1
* 작성자: Sevenshards
* 작성 일자: 2023-12-15
* 이전 버전 작성 일자:
* 버전 내용: 간단한 소켓 프로그래밍 예제 - Server, Windows, MBCS기반
* 이전 버전 내용:
*/

#pragma comment(lib, "ws2_32.lib")

#include <stdio.h>
#include <stdlib.h>
#include <WinSock2.h>

void ErrorHandling(const char* message);

int main(int argc, char* argv[])
{
	WSADATA wsaData; // 큰 의미는 없으나 사용해야 함
	SOCKET hServSock, hClntSock; // 소켓 핸들 (리눅스의 파일 디스크립터)
	SOCKADDR_IN servAddr, clntAddr;

	int szClntAddr;
	char message[] = "Hello World!";
	if (argc != 2)
	{
		printf("Usage: %s <Port>\n", argv[0]);
		exit(1);
	}

	if(WSAStartup(MAKEWORD(2, 2), &wsaData) !=0) // WinSock 라이브러리 초기화, 소켓 버전 2.2
		ErrorHandling("WSAStartup() error!");

	hServSock = socket(PF_INET, SOCK_STREAM, 0); // 리눅스의 socket 함수와 동일, 소켓 (핸들)생성
	if (hServSock == INVALID_SOCKET)
		ErrorHandling("socket() error");

	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAddr.sin_port = htons(atoi(argv[1]));

	if (bind(hServSock, (SOCKADDR*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR) // 리눅스의 bind와 동일, 소켓에 주소 정보 할당
		ErrorHandling("bind() error");

	if (listen(hServSock, 5) == SOCKET_ERROR) // 리눅스의 listen과 동일, 소켓 통신 대기 상태
		ErrorHandling("listen() error");

	szClntAddr = sizeof(clntAddr);
	hClntSock = accept(hServSock, (SOCKADDR*)&clntAddr, &szClntAddr); // 리눅스의 accept와 동일, 소켓 통신 수락
	if (hClntSock == INVALID_SOCKET)
		ErrorHandling("accept() error");

	send(hClntSock, message, sizeof(message), 0); // 리눅스의 write
	closesocket(hClntSock); // 리눅스에서는 파일과 소켓이 동일하므로 close
	closesocket(hServSock); // Windows에서는 파일과 소켓은 다르므로 closesocket이 따로 있음. CloseHandle과 비슷하다고 보면 될 듯.
	WSACleanup(); // WinSock 라이브러리 해제, OS에 반환

	return 0;
}

void ErrorHandling(const char* message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}