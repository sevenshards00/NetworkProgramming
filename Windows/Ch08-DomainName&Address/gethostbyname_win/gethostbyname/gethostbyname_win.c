/*
* TCP/IP Socket Programming - 도메인 이름(Domain Name)과 인터넷 주소(IP Address)
* 파일명: gethostbyname_win.c
* 파일 버전: 0.1
* 작성자: Sevenshards
* 작성 일자: 2023-12-20
* 이전 버전 작성 일자:
* 버전 내용: Windows 기반 도메인 이름을 통한 IP 주소 가져오기(gethostbyname)
* 이전 버전 내용:
*/

#pragma comment(lib, "ws2_32.lib")
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

void ErrorHandling(const char* err_msg);

int main(int argc, char* argv[])
{
    WSADATA wsaData;
    int i;
    HOSTENT* host;

    char ip_addr[20] = { 0, };
    if (argc != 2)
    {
        printf("Usage: %s <Hostname>\n", argv[0]);
        exit(1);
    }
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        ErrorHandling("WSAStartup error");

    // host = gethostbyname(argv[1]);
    // gethostbyname()은 deprecated
    // 굳이 사용한다면 #define _WINSOCK_DEPRECATED_NO_WARNINGS를 사용할 것
    // getaddrinfo 또는 getAddrInfo를 사용할 것을 권장
    // 쓰는 방법이 좀 까다로워서 아예 코드를 따로 빼놓고 예제를 남겨둬야겠음.
    // 완전 다른 느낌임.
    host = gethostbyname(argv[1]);
    if (!host)
        ErrorHandling("can't find host");

    printf("Official name: %s\n", host->h_name);
    for (i = 0; host->h_aliases[i]; i++)
        printf("Aliases %d: %s\n", i + 1, host->h_aliases[i]);
    printf("Address type: %s\n", (host->h_addrtype == AF_INET) ? "AF_INET" : "AF_INET6");
    for (i = 0; host->h_addr_list[i]; i++)
    {
        // 이상한 부분에서 헤메고 있었다.
        // h_addr_list[i]는 char* -> 즉 IP 주소
        // 단, 네트워크 바이트 순서로 되어 있으므로 이를 바꿔줘야 함
        inet_ntop(host->h_addrtype, host->h_addr_list[i], ip_addr, sizeof(ip_addr));
        printf("IP Addr %d: %s\n", i + 1, ip_addr);
    }
    return 0;

    WSACleanup();
    return 0;
}

void ErrorHandling(const char* err_msg)
{
    fputs(err_msg, stderr);
    fputc('\n', stderr);
    exit(1);
}