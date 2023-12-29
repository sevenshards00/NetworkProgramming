/*
* TCP/IP Socket Programming - Windows에서의 쓰레드 동기화
* 파일명: chat_clnt_win.cpp
* 파일 버전: 0.1
* 작성자: Sevenshards
* 작성 일자: 2023-12-29
* 이전 버전 작성 일자:
* 버전 내용: Windows 기반 멀티 쓰레드 서버 구현 (채팅 클라이언트)
* 이전 버전 내용:
*/

#pragma comment (lib, "ws2_32.lib")

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
// 헤더파일 추가 포함시 순서도 약간 중요
// Windows.h를 먼저 포함할 경우 매크로 재정의 및 함수 재정의의 문제가 발생한다.
// 그래서 다음과 같은 순서로 헤더파일을 추가할 것.
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>
#include <process.h>
using std::cin;
using std::cout;

#define BUF_SIZE 1024
#define IPV4_SIZE 32
#define NAME_SIZE 20

unsigned WINAPI SendMsg(void* arg); // 송신용 쓰레드의 main이 될 함수
unsigned WINAPI RecvMsg(void* arg); // 수신용 쓰레드의 main이 될 함수
void ErrorHandling(const char* err_msg);

char name[NAME_SIZE] = "[DEFAULT]";
char msg[BUF_SIZE];

int main(int argc, char* argv[])
{
    WSADATA wsaData;
    SOCKET hSock;
    SOCKADDR_IN servAddr;
    // 2개의 쓰레드를 사용한다.
    // 송신용 쓰레드, 수신용 쓰레드.
    HANDLE hSendThread, hRecvThread;

    if (argc != 4)
    {
        cout << "Usage:" << argv[0] << " <IP> <Port> <Nickname>\n";
        exit(1);
    }

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        ErrorHandling("WSAStartup() error");

    // name의 문자열을 입력받은 값으로 대체
    // sprintf는 포맷에 맞춰서 문자열을 저장
    sprintf_s(name, sizeof(name), "[%s]", argv[3]);
    hSock = socket(PF_INET, SOCK_STREAM, 0);
    if (hSock == INVALID_SOCKET)
        ErrorHandling("socket() error");

    ZeroMemory(&servAddr, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    if (inet_pton(AF_INET, argv[1], &servAddr.sin_addr) == SOCKET_ERROR)
        ErrorHandling("failed init address error");
    servAddr.sin_port = htons(atoi(argv[2]));

    if (connect(hSock, (SOCKADDR*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR)
        ErrorHandling("connect() error");

    // connect까지 완료가 되었다면 쓰레드를 생성
    hSendThread = (HANDLE)_beginthreadex(NULL, 0, SendMsg, (void*)&hSock, 0, NULL);
    hRecvThread = (HANDLE)_beginthreadex(NULL, 0, RecvMsg, (void*)&hSock, 0, NULL);
    
    // WaitForSingleObject 함수를 호출하여 쓰레드가 종료될 때까지 블로킹.
    // Linux와의 차이점은 Windows에서 쓰레드는 return을 하면 종료와 동시에 소멸
    WaitForSingleObject(hSendThread, INFINITE);
    WaitForSingleObject(hRecvThread, INFINITE);
    
    closesocket(hSock);
    WSACleanup();
    return 0;
}

// 송신용 쓰레드의 main이 될 함수
unsigned WINAPI SendMsg(void* arg)
{
    SOCKET hSock = *((SOCKET*)arg);
    char nameMsg[NAME_SIZE + BUF_SIZE];
    while (1)
    {
        fgets(msg, BUF_SIZE, stdin);
        if (!strcmp(msg, "q\n") || !strcmp(msg, "Q\n"))
        {
            closesocket(hSock);
            exit(0);
        }
        // "[Name] 입력한 문자열"
        sprintf_s(nameMsg, sizeof(nameMsg), "%s %s", name, msg);
        send(hSock, nameMsg, strlen(nameMsg), 0);
    }
    return 0;
}

// 수신용 쓰레드의 main이 될 함수
unsigned WINAPI RecvMsg(void* arg)
{
    SOCKET hSock = *((SOCKET*)arg);
    char nameMsg[NAME_SIZE + BUF_SIZE];
    int strLen;
    while (1)
    {
        strLen = recv(hSock, nameMsg, NAME_SIZE + BUF_SIZE - 1, 0);
        if (strLen == -1)
            return -1;
        nameMsg[strLen] = '\0';
        fputs(nameMsg, stdout);
    }
    return 0;
}

void ErrorHandling(const char* err_msg)
{
    fputs(err_msg, stderr);
    fputc('\n', stderr);
    exit(1);
}