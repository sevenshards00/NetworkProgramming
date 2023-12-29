/*
* TCP/IP Socket Programming - Windows에서의 쓰레드 동기화
* 파일명: chat_serv_win.cpp
* 파일 버전: 0.1
* 작성자: Sevenshards
* 작성 일자: 2023-12-29
* 이전 버전 작성 일자:
* 버전 내용: Windows 기반 멀티 쓰레드 서버 구현 (채팅 서버)
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
#define MAX_CLNT 256

unsigned WINAPI HandleClnt(void* arg); // 쓰레드에서 사용될 함수. 클라이언트 관리에 이용
void SendMsg(char* msg, int len); // 메세지를 보낼 때 사용할 함수
void ErrorHandling(const char* err_msg);

// Client의 소켓 관리를 위한 변수와 배열
INT clntCnt = 0;
SOCKET clntSocks[MAX_CLNT];
HANDLE hMutex;

int main(int argc, char* argv[])
{
    WSADATA wsaData;
    SOCKET hServSock, hClntSock;
    SOCKADDR_IN servAddr, clntAddr;
    char clntAddrBuf[IPV4_SIZE] = { 0, };
    INT clntAddrSize;
    HANDLE hThread;

    if (argc != 2)
    {
        cout << "Usage: " << argv[0] << " <Port>\n";
        exit(1);
    }

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        ErrorHandling("WSAStartup() error");

    hMutex = CreateMutex(NULL, FALSE, NULL);
    hServSock = socket(PF_INET, SOCK_STREAM, 0);
    if (hServSock == INVALID_SOCKET)
        ErrorHandling("socket() error");

    ZeroMemory(&servAddr, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(atoi(argv[1]));

    if (bind(hServSock, (SOCKADDR*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR)
        ErrorHandling("bind() error");
    if (listen(hServSock, 5) == SOCKET_ERROR)
        ErrorHandling("listen() error");

    while (1)
    {
        clntAddrSize = sizeof(clntAddr);
        hClntSock = accept(hServSock, (SOCKADDR*)&clntAddr, &clntAddrSize);
        if (hClntSock == INVALID_SOCKET)
            ErrorHandling("accept() error");

        // 소켓 생성 이후 연결이 생성될 때마다 clnt_cnt를 증가
        // 배열에 소켓의 핸들값을 등록
        // Windows에서는 소켓 역시 커널 오브젝트이므로
        // 임계영역으로 둔 이유 -> 전역 변수 -> 쓰레드에 의한 동시 접근 가능
        WaitForSingleObject(hMutex, INFINITE);
        clntSocks[clntCnt++] = hClntSock;
        ReleaseMutex(hMutex);

        // 쓰레드 생성
        // 전달하는 인자는 클라이언트 소켓의 핸들값
        // Linux와의 차이점은 Windows에서 쓰레드는 return을 하면 종료와 동시에 소멸
        // 별도로 _endthreadex를 사용할 수도 있으나 이는 강제 종료이므로 쓸 이유가 없음
        hThread = (HANDLE)_beginthreadex(NULL, 0, HandleClnt, (void*)&hClntSock, 0, NULL);
        if(inet_ntop(AF_INET, &clntAddr.sin_addr, clntAddrBuf, sizeof(clntAddrBuf)) == NULL)
            ErrorHandling("inet_ntop() error");
        cout << "Connected Client IP: " << clntAddrBuf << '\n';
    }

    closesocket(hServSock);
    WSACleanup();

	return 0;
}

unsigned WINAPI HandleClnt(void* arg) // 쓰레드에서 사용될 함수. 클라이언트 관리에 이용
{
    SOCKET hClntSock = *((SOCKET*)arg);
    int strLen = 0, i;
    char msg[BUF_SIZE];

    // EOF를 받기 전까지 계속 입력받는 메시지를 전송
    while ((strLen = recv(hClntSock, msg, sizeof(msg), 0)) != 0)
        SendMsg(msg, strLen);

    // 메시지 전송이 끝난 클라이언트의 접속 종료 과정
    WaitForSingleObject(hMutex, INFINITE);
    for (i = 0; i < clntCnt; i++)
    {
        // 현재 클라이언트 소켓과 배열에 등록된 클라이언트 소켓을 비교
        if (hClntSock == clntSocks[i])
        {
            // 접속이 종료된 클라이언트의 자리만큼 배열을 재배치
            // 쉽게 말하면 배열의 요소를 한 칸씩 앞으로 당긴다.
            // 이 부분은 연결 리스트나 다른 자료구조를 사용할 수 있다면 깔끔할 수도 있을듯.
            while (i++ < clntCnt - 1)
                clntSocks[i] = clntSocks[i + 1];
            break;
        }
    }
    clntCnt--; // 클라이언트가 접속을 종료하였으므로 클라이언트의 Count 감소
    ReleaseMutex(hMutex);
    closesocket(hClntSock);
    return 0;
}

// 현재 연결된 모든 클라이언트에게 메시지를 전송
void SendMsg(char* msg, int len) // 메세지를 보낼 때 사용할 함수
{
    int i;
    WaitForSingleObject(hMutex, INFINITE);
    // for문을 통해 등록된 클라이언트 소켓에 메시지를 send한다.
    for (i = 0; i < clntCnt; i++)
        send(clntSocks[i], msg, len, 0);
    ReleaseMutex(hMutex);
}

void ErrorHandling(const char* err_msg)
{
    fputs(err_msg, stderr);
    fputc('\n', stderr);
    exit(1);
}