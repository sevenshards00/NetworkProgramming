/*
* TCP/IP Socket Programming - 다양한 입출력 함수들
* 파일명: oob_send_win.c
* 파일 버전: 0.1
* 작성자: Sevenshards
* 작성 일자: 2023-12-26
* 이전 버전 작성 일자:
* 버전 내용: Windows 기반 send 함수를 이용한 MSG_OOB(Out-Of-Band Data) 옵션
* 이전 버전 내용:
*/

#pragma comment (lib, "ws2_32.lib")

#include <stdio.h>
#include <stdlib.h>
#include <WinSock2.h>

#define BUF_SIZE 30
void ErrorHandling(const char* err_msg);

int main(int argc, char* argv[])
{
    WSADATA wsaData;
    SOCKET hSock;
    SOCKADDR_IN sendAddr;

    if (argc != 3)
    {
        printf("Usage: %s <IP> <Port>", argv[0]);
        exit(1);
    }

    if (WSAStartup(MAKEWORD(2, 2), &wsaData))
        ErrorHandling("WSAStartup() error");

    hSock = socket(PF_INET, SOCK_STREAM, 0);
    if (hSock == INVALID_SOCKET)
        ErrorHandling("socket() error");

    ZeroMemory(&sendAddr, sizeof(sendAddr));
    sendAddr.sin_family = AF_INET;
    if (inet_pton(AF_INET, argv[1], &sendAddr.sin_addr) == SOCKET_ERROR)
        ErrorHandling("failed init address error");
    sendAddr.sin_port = htons(atoi(argv[2]));

    if (connect(hSock, (SOCKADDR*)&sendAddr, sizeof(sendAddr)) == SOCKET_ERROR)
        ErrorHandling("connect() error");

    send(hSock, "123", strlen("123"), 0);
    send(hSock, "4", strlen("4"), MSG_OOB); // 긴급 데이터 (MSG_OOB) 전송
    send(hSock, "567", strlen("567"), 0);
    send(hSock, "890", strlen("890"), MSG_OOB); // 긴급 데이터 (MSG_OOB) 전송

    closesocket(hSock);
    WSACleanup();
    return 0;
}

void ErrorHandling(const char* err_msg)
{
    fputs(err_msg, stderr);
    fputc('\n', stderr);
    exit(1);
}