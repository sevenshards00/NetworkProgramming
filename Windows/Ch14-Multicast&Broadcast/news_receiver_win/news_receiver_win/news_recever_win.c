/*
* TCP/IP Socket Programming - 멀티캐스트 & 브로드캐스트
* 파일명: news_receiver_win.c
* 파일 버전: 0.1
* 작성자: Sevenshards
* 작성 일자: 2023-12-27
* 이전 버전 작성 일자:
* 버전 내용: Windows 기반 멀티캐스트 예제 - receiver
* 이전 버전 내용:
*/

#pragma comment (lib, "ws2_32.lib")

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

#define BUF_SIZE 30

void ErrorHandling(const char* err_msg);

int main(int argc, char* argv[])
{
    WSADATA wsaData;
    SOCKET hRecvSock;
    int str_len;
    char buf[BUF_SIZE];
    SOCKADDR_IN addr;
    IP_MREQ joinAddr; // 멀티캐스트 그룹 관련 IP_MREQ형 구조체

    if (argc != 3)
    {
        printf("Usage: %s <GroupIP> <Port>\n", argv[0]);
        exit(1);
    }

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        ErrorHandling("WSAStartup() error");

    // 멀티캐스트와 브로드캐스트는 UDP 기반
    // 그래서 소켓도 UDP 소켓 생성
    hRecvSock = socket(PF_INET, SOCK_DGRAM, 0);
    if (hRecvSock == INVALID_SOCKET)
        ErrorHandling("socket() error");

    ZeroMemory(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(atoi(argv[2]));

    if (bind(hRecvSock, (SOCKADDR*)&addr, sizeof(addr)) == SOCKET_ERROR)
        ErrorHandling("bind() error");
    
    // IP_MREQ형 구조체에 멀티캐스트 그룹 가입을 위해 주소 입력
    // 멤버인 imr_multiaddr에는 가입할 멀티캐스트 그룹의 IP 주소
    // 멤버 imr_interface에는 그룹에 가입할 소켓이 속한 호스트의 IP 주소 명시 -> INADDR_ANY를 사용
    if (inet_pton(AF_INET, argv[1], &joinAddr.imr_multiaddr.s_addr) == -1)
        ErrorHandling("failed init address error");
    joinAddr.imr_interface.s_addr = htonl(INADDR_ANY);

    // 멀티캐스트 그룹에 가입하는 것 또한 소켓의 옵션 설정을 통해 이뤄짐
    // TTL 설정과 마찬가지로 프로토콜 레벨은 IPPROTO_IP
    // 옵션의 이름은 IP_ADD_MEMBERSHIP
    setsockopt(hRecvSock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (void*)&joinAddr, sizeof(joinAddr));

    while (1)
    {
        str_len = recvfrom(hRecvSock, buf, BUF_SIZE - 1, 0, NULL, 0);
        if (str_len < 0)
            break;
        buf[str_len] = '\0';
        fputs(buf, stdout);
    }
    closesocket(hRecvSock);
    WSACleanup();
    return 0;
}

void ErrorHandling(const char* err_msg)
{
    fputs(err_msg, stderr);
    fputc('\n', stderr);
    exit(1);
}