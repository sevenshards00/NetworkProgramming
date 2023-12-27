/*
* TCP/IP Socket Programming - 멀티캐스트 & 브로드캐스트
* 파일명: news_sender_win.c
* 파일 버전: 0.1
* 작성자: Sevenshards
* 작성 일자: 2023-12-27
* 이전 버전 작성 일자:
* 버전 내용: Windows 기반 멀티캐스트 예제 - sender
* 이전 버전 내용:
*/

#pragma comment (lib, "ws2_32.lib")

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

#define TTL 64 // Time-To-Live
#define BUF_SIZE 30

void ErrorHandling(const char* err_msg);

int main(int argc, char* argv[])
{
    WSADATA wsaData;
    SOCKET hSendSock;
    SOCKADDR_IN mulAddr;
    int time_live = TTL;
    FILE* fp = NULL;
    char buf[BUF_SIZE];

    if (argc != 3)
    {
        printf("Usage: %s <GroupIP> <Port>\n", argv[0]);
        exit(1);
    }

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        ErrorHandling("WSAStartup() error");

    // 멀티캐스트와 브로드캐스트는 UDP 기반
    // 소켓 생성 시 UDP 소켓으로 생성
    hSendSock = socket(PF_INET, SOCK_DGRAM, 0);
    if (hSendSock == INVALID_SOCKET)
        ErrorHandling("socket() error");

    ZeroMemory(&mulAddr, sizeof(mulAddr));
    mulAddr.sin_family = AF_INET;
    if (inet_pton(AF_INET, argv[1], &mulAddr.sin_addr) == -1)
        ErrorHandling("failed init address error");
    mulAddr.sin_port = htons(atoi(argv[2]));

    // 소켓의 옵션 지정
    // TTL 관련 설정 프로토콜의 레벨은 IPPROTO_IP
    // 옵션명은 IP_MULTICAST_TTL
    setsockopt(hSendSock, IPPROTO_IP, IP_MULTICAST_TTL, (void*)&time_live, sizeof(time_live));
    
    fopen_s(&fp, "news.txt", "r");
    if (fp == NULL)
        ErrorHandling("fopen() error");

    while (!feof(fp))
    {
        fgets(buf, BUF_SIZE, fp);
        sendto(hSendSock, buf, strlen(buf), 0, (SOCKADDR*)&mulAddr, sizeof(mulAddr));
        Sleep(2000);
    }

    fclose(fp);
    closesocket(hSendSock);
    WSACleanup();
    return 0;
}

void ErrorHandling(const char* err_msg)
{
    fputs(err_msg, stderr);
    fputc('\n', stderr);
    exit(1);
}