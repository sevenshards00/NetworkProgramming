/*
* TCP/IP Socket Programming - 도메인 이름(Domain Name)과 인터넷 주소(IP Address)
* 파일명: getaddrinfo.c
* 파일 버전: 0.1
* 작성자: Sevenshards
* 작성 일자: 2023-12-20
* 이전 버전 작성 일자:
* 버전 내용: Windows 기반 도메인 이름과 포트 번호를 통한 IP 주소 가져오기(getaddrinfo)
* 이전 버전 내용:
*/

#pragma comment(lib, "ws2_32.lib")

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

void ErrorHandling(const char* err_msg);

int main(int argc, char* argv[])
{
    WSADATA wsaData;
    ADDRINFO* host;
    char ip_addr[20];
    if (argc != 3)
    {
        printf("Usage: %s <Hostname> <Port>\n", argv[0]);
        exit(1);
    }
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        ErrorHandling("WSAStartup error");

    // host = gethostbyname(argv[1]);
    // gethostbyname()은 deprecated
    // 굳이 사용한다면 #define _WINSOCK_DEPRECATED_NO_WARNINGS를 사용할 것
    // getaddrinfo 또는 getAddrInfo를 사용할 것을 권장
    getaddrinfo(
        argv[1], // 첫 번째 인자에는 도메인 이름이 들어간다.
        argv[2], // 두 번째 인자에는 포트 번호를 넣는다.
        NULL, // 세 번째 인자에는 힌트(hint)를 인자로 보낸다.
        &host // 네 번째 인자에는 결과 값을 받을 변수의 주소값을 넣는다.
    );
    if (!host)
        ErrorHandling("can't find host");
    
    printf("CName: %s\n", host->ai_canonname);
    printf("Address type: %s\n", (host->ai_family == AF_INET) ? "AF_INET" : "AF_INET6");
    while (host->ai_next != NULL)
    {
        // IP주소는 sa_data의 두 번째 인덱스
        // 나중에 여유가 된다면 sa_data의 인덱스별로 어떤 데이터가 있는지 알아볼 것
        inet_ntop(host->ai_family, (char*)&host->ai_addr->sa_data[2], ip_addr, sizeof(ip_addr));
        printf("IP Addr: %s\n", ip_addr);
        host = host->ai_next;
    }

    freeaddrinfo(host);
    WSACleanup();
    return 0;
}

void ErrorHandling(const char* err_msg)
{
    fputs(err_msg, stderr);
    fputc('\n', stderr);
    exit(1);
}