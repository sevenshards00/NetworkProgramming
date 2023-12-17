/*
* TCP/IP Socket Programming - 주소체계와 데이터 정렬
* 파일명: inet_addr_win.c
* 파일 버전: 0.1
* 작성자: Sevenshards
* 작성 일자: 2023-12-17
* 이전 버전 작성 일자:
* 버전 내용: inet_addr 함수를 이용한 문자열 ip주소를 네트워크 바이트 순서의 정수로 변환하는 예제
* 이전 버전 내용:
*/

#pragma comment(lib, "ws2_32.lib")

#include <stdio.h>
#include <string.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

void ErrorHandling(const char* err_msg);

int main(int argc, char* argv[])
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		ErrorHandling("WSAStartup error");

	{
		const char* addr = "127.212.124.78";
		unsigned long conv_addr = inet_addr(addr);
		if (conv_addr == INADDR_NONE)
			printf("Error Occured!\n");
		else
			printf("Network ordered integer addr: %#lx\n", conv_addr);
	}
	{
		struct sockaddr_in addr;
		char strArr[20];

		addr.sin_addr.s_addr = htonl(0x01020304);
		// strPtr = inet_ntoa(addr.sin_addr);
		// inet_addr과 마찬가지로 deprecated되어 곧 쓰지 않을 수 있음
		// 따라서 inet_ntop() 또는 InetNtoP()를 사용할 것을 권장 -> header에 WS2tcpip.h 추가 필요
		// ntop == network to pc / pton == pc to network
		// pton때와 마찬가지로 #define _WINSOCK_DEPRECATED_NO_WARNINGS를 사용해도 되지만 권장하는 방법으로 구현
		inet_ntop(AF_INET, &addr.sin_addr, strArr, sizeof(strArr));
		printf("Dotted-Decimal notation %s\n", strArr);
	}

	WSACleanup();
	return 0;
}

void ErrorHandling(const char* err_msg)
{
	fputs(err_msg, stderr);
	fputc('\n', stderr);
	exit(1);
}