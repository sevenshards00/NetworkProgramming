/*
* TCP/IP Socket Programming - 도메인 이름(Domain Name)과 인터넷 주소(IP Address)
* 파일명: gethostbyaddr_win.c
* 파일 버전: 0.1
* 작성자: Sevenshards
* 작성 일자: 2023-12-20
* 이전 버전 작성 일자:
* 버전 내용: Windows 기반 주소를 통한 도메인 이름 가져오기(gethostbyaddr)
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
    SOCKADDR_IN addr;
    char ip_addr[20];
    
    if (argc != 2)
    {
        printf("Usage: %s <IP>\n", argv[0]);
        exit(1);
    }

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        ErrorHandling("WSAStartup() error");

    ZeroMemory(&addr, sizeof(addr));
    inet_pton(AF_INET, argv[1], &addr.sin_addr);
    // gethostbyaddr()도 마찬가지로 deprecated
    // 굳이 사용한다면 #define _WINSOCK_DEPRECATED_NO_WARNINGS를 사용할 것
    // getnameinfo 또는 getNameInfo를 사용할 것을 권장
    // 위 두 함수가 IP주소값을 사용해서 도메인 이름을 가져오는 함수다.
    // getaddrinfo보다는 사용하기 간편하다.
    // 다만 이 코드에서 완벽하게 대체하기는 어려우므로 이것도 아예 코드를 따로 빼놓고 예제를 남겨둬야겠음.
    host = gethostbyaddr((char*)&addr.sin_addr, sizeof(addr.sin_addr.s_addr), AF_INET);
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

    WSACleanup();
    return 0;
}

void ErrorHandling(const char* err_msg)
{
    fputs(err_msg, stderr);
    fputc('\n', stderr);
    exit(1);
}