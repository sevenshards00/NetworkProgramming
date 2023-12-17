/*
* TCP/IP Socket Programming - 주소체계와 데이터 정렬
* 파일명: endian_conv_win.c
* 파일 버전: 0.1
* 작성자: Sevenshards
* 작성 일자: 2023-12-17
* 이전 버전 작성 일자:
* 버전 내용: Host Ordered(little endian), Network Ordered(big endian) 변환 예제
* 이전 버전 내용:
*/

#pragma comment(lib, "ws2_32.lib")

#include <stdio.h>
#include <WinSock2.h>


void ErrorHandling(const char* err_msg);

int main(int argc, char* argv[])
{
	WSADATA wsaData;
	unsigned short host_port = 0x1234;
	unsigned short net_port;
	unsigned long host_addr = 0x12345678;
	unsigned long net_addr;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		ErrorHandling("WSAStartup() error");

	net_port = htons(host_port);
	net_addr = htonl(host_addr);

	printf("Host ordered port: %#x\n", host_port);
	printf("Network ordered port: %#x\n", net_port);
	printf("Host ordered Address: %#x\n", host_addr);
	printf("Network ordered port: %#x\n", net_addr);
	WSACleanup();
	return 0;
}

void ErrorHandling(const char* err_msg)
{
	fputs(err_msg, stderr);
	fputc('\n', stderr);
	exit(1);
}