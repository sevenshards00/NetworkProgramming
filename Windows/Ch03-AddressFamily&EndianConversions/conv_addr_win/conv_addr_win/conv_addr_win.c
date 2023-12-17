/*
* TCP/IP Socket Programming - 주소체계와 데이터 정렬
* 파일명: conv_addr_win.c
* 파일 버전: 0.1
* 작성자: Sevenshards
* 작성 일자: 2023-12-17
* 이전 버전 작성 일자:
* 버전 내용: WSAStringToAddressW, WSAAddressToStringW 사용 예제
* 이전 버전 내용:
*/

#pragma comment(lib, "ws2_32.lib")

#include <stdio.h>
#include <tchar.h>
#include <WinSock2.h>

void ErrorHandling(CONST TCHAR* err_msg);

int _tmain(int argc, TCHAR* argv[])
{
	CONST TCHAR* strAddr = TEXT("203.211.218.102:9190"); // 포트 번호까지 포함한 주소

	TCHAR strAddrBuf[50];
	SOCKADDR_IN servAddr;
	DWORD size;

	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		ErrorHandling(TEXT("WSAStartup error"));

	// 문자열을 주소로 담는 과정 (WSAStringToAddress, Windows 기반의 inet_aton)
	// WSAStringToAddressW를 사용하라고 함
	// 마찬가지로 deprecated될 함수, 있다는 것만 알아두고 ntop와 pton 위주로 사용하는 것으로.
	size = sizeof(servAddr);
	WSAStringToAddressW(strAddr, AF_INET, NULL, (SOCKADDR*)&servAddr, &size);

	// 주소를 문자열로 담는 과정(WSAAddressToString, Windows 기반의 inet_ntoa)
	// WSAAddressToStringW를 사용하라고 함
	// 마찬가지로 deprecated될 함수, 있다는 것만 알아두고 ntop와 pton 위주로 사용하는 것으로.
	size = sizeof(strAddrBuf);
	WSAAddressToStringW((SOCKADDR*)&servAddr, sizeof(servAddr), NULL, strAddrBuf, &size);
	_tprintf(TEXT("Second Conversion result: %s\n"), strAddrBuf);

	WSACleanup();
	return 0;
}

void ErrorHandling(CONST TCHAR* err_msg)
{
	fputs(err_msg, stderr);
	fputc('\n', stderr);
	exit(1);
}