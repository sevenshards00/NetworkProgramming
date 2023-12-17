/*
* TCP/IP Socket Programming - TCP 기반 서버/클라이언트 (1)
* 파일명: echo_server_win.c
* 파일 버전: 0.1
* 작성자: Sevenshards
* 작성 일자: 2023-12-17
* 이전 버전 작성 일자:
* 버전 내용: TCP 기반 Iterative Echo Server
* 이전 버전 내용:
*/

#pragma comment(lib, "ws2_32.lib")

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

void ErrorHandling(const char* err_msg);

#define BUF_SIZE 1024

int main(int argc, char* argv[])
{
	WSADATA wsaData;
	SOCKET hServSock, hClntSock; // 서버 소켓, 클라이언트 소켓
	char message[BUF_SIZE];
	int strLen, i;

	SOCKADDR_IN servAddr, clntAddr; // 서버, 클라이언트의 주소를 담을 구조체, IPv4 기준
	int clntAddrSize; // 클라이언트의 주소 크기

	if (argc != 2)
	{
		printf("Usage: %s <Port>\n", argv[0]);
		exit(1);
	}
		

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		ErrorHandling("WSAStartup() Error");

	hServSock = socket(PF_INET, SOCK_STREAM, 0); // 서버 소켓 생성
	if (hServSock == INVALID_SOCKET)
		ErrorHandling("socket() Error");

	memset(&servAddr, 0, sizeof(servAddr)); // servAddr의 구조체의 메모리를 0으로 초기화
	servAddr.sin_family = AF_INET; // IPv4, IPv6라면 AF_INET6
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY); // INADDR_ANY는 서버 측에서 사용. 컴퓨터의 IP 주소를 자동으로 할당
	servAddr.sin_port = htons(atoi(argv[1])); // Port

	// 서버 소켓에 주소를 할당(bind)
	if (bind(hServSock, (SOCKADDR*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR)
		ErrorHandling("bind() Error");

	// 서버 소켓을 리스닝 소켓으로 -> 연결 요청 대기상태로
	// 두 번째 인자는 연결 요청 대기 큐의 크기
	if (listen(hServSock, 5) == SOCKET_ERROR)
		ErrorHandling("listen() Error");

	// 클라이언트의 주소값의 크기를 대입
	clntAddrSize = sizeof(clntAddr);

	for (i = 0; i < 5; i++)
	{
		// accept를 통해 연결을 수락.
		hClntSock = accept(hServSock, (SOCKADDR*)&clntAddr, &clntAddrSize);
		if (hClntSock == -1)
			ErrorHandling("accept() Error");
		else
			printf("Connected client %d\n", i + 1);

		// 클라이언트로부터 읽을 메시지가 없을 때까지 받아들인다(recv)
		while ((strLen = recv(hClntSock, message, BUF_SIZE, 0)) != 0)
			send(hClntSock, message, strLen, 0); // 클라이언트에게 받은 메시지를 다시 전송

		closesocket(hClntSock); // 연결이 종료, 클라이언트 소켓을 닫는다.
	}
	closesocket(hServSock); // 서버 통신도 종료되면 서버 소켓도 닫는다.
	WSACleanup();
	return 0;
}

void ErrorHandling(const char* err_msg)
{
	fputs(err_msg, stderr);
	fputc('\n', stderr);
	exit(1);
}