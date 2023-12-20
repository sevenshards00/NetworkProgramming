/*
* TCP/IP Socket Programming - 소켓의 연결 종료
* 파일명: file_client_win.c
* 파일 버전: 0.1
* 작성자: Sevenshards
* 작성 일자: 2023-12-20
* 이전 버전 작성 일자:
* 버전 내용: 파일 전송 기반 Half-close 예시 - Client
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
    SOCKET hSock;
    FILE* fp;

    char buf[BUF_SIZE];
    int read_cnt;
    SOCKADDR_IN serv_addr;
    if (argc != 3)
    {
        printf("Usage: %s <IP> <Port>\n", argv[0]);
        exit(1);
    }

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        ErrorHandling("WSAStartup() error");
    // fp = fopen("receive.dat", "wb");
    fopen_s(&fp, "receive.dat", "wb"); // 서버에서 보내는 데이터를 담기 위해 파일을 생성.
    hSock = socket(PF_INET, SOCK_STREAM, 0);
    if (hSock == INVALID_SOCKET)
        ErrorHandling("socket() error");

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    if (inet_pton(AF_INET, argv[1], &serv_addr.sin_addr) == SOCKET_ERROR)
        ErrorHandling("fail init address");
    serv_addr.sin_port = htons(atoi(argv[2]));

    if (connect(hSock, (SOCKADDR*)&serv_addr, sizeof(serv_addr)) == SOCKET_ERROR)
        ErrorHandling("connect error");
    else
        puts("Connected.......");

    // 서버에서 데이터를 얼마나 전송할지 모른다.
    // 그래서 read 함수를 호출하고 반환하는 값이 0이면 더 이상 서버에서 보내는 데이터가 없는 것
    // 다시 말해서 서버에서 보내는 데이터가 없을 때까지 계속 read를 한다
    // 현재 코드를 기준으로는 EOF가 전송될 때까지 읽는 것
    while ((read_cnt = recv(hSock, buf, BUF_SIZE, 0)) != 0)
        fwrite((void*)buf, 1, read_cnt, fp); // 서버에서 보내는 데이터를 EOF가 전송될 때까지 파일에 저장.

    fputs("Received file data\n", stdout);
    send(hSock, "Thank you", 10, 0); // 마지막으로 연결을 종료하기 전 서버에 메시지를 보낸다.
    fclose(fp); // 파일 스트림을 닫고
    closesocket(hSock); // 소켓 연결 종료
    WSACleanup();
    return 0;
}

void ErrorHandling(const char* err_msg)
{
    fputs(err_msg, stderr);
    fputc('\n', stderr);
    exit(1);
}