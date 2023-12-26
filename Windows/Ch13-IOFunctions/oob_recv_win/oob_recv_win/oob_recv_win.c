/*
* TCP/IP Socket Programming - 다양한 입출력 함수들
* 파일명: oob_recv_win.c
* 파일 버전: 0.1
* 작성자: Sevenshards
* 작성 일자: 2023-12-26
* 이전 버전 작성 일자:
* 버전 내용: Windows 기반 recv 함수를 이용한 MSG_OOB(Out-Of-Band Data) 옵션
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
    SOCKET hAcptSock, hRecvSock;
    SOCKADDR_IN recvAddr, sendAddr;
    int strLen, sendAddrSz;
    char buf[BUF_SIZE];
    int result;

    // select 함수를 사용하기 위한 fd_set 변수 선언 및 timeout 구조체 선언
    fd_set read, except, readCopy, exceptCopy;
    struct timeval timeout;

    if (argc != 2)
    {
        printf("Usage: %s <Port>", argv[0]);
        exit(1);
    }

    if (WSAStartup(MAKEWORD(2, 2), &wsaData))
        ErrorHandling("WSAStartup() error");

    hAcptSock = socket(PF_INET, SOCK_STREAM, 0);
    if (hAcptSock == INVALID_SOCKET)
        ErrorHandling("socket() error");

    ZeroMemory(&recvAddr, sizeof(recvAddr));
    recvAddr.sin_family = AF_INET;
    recvAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    recvAddr.sin_port = htons(atoi(argv[1]));

    if (bind(hAcptSock, (SOCKADDR*)&recvAddr, sizeof(recvAddr)) == SOCKET_ERROR)
        ErrorHandling("bind() error");

    if (listen(hAcptSock, 5) == SOCKET_ERROR)
        ErrorHandling("listen() error");

    sendAddrSz = sizeof(sendAddr);
    hRecvSock = accept(hAcptSock, (SOCKADDR*)&sendAddr, &sendAddrSz);
    if (hRecvSock == SOCKET_ERROR)
        ErrorHandling("accept() error");
    
    // Windows에는 Signal을 사용하지 않음
    // 대신 select를 이용하여 예외상황이 발생하는 소켓을 확인한다
    // 다시 말해서 MSG_OOB가 전달되었다는 것은 예외상황의 발생을 의미
    FD_ZERO(&read);
    FD_ZERO(&except);
    FD_SET(hRecvSock, &read);
    FD_SET(hRecvSock, &except);
    
    while (1)
    {
        readCopy = read;
        exceptCopy = except;
        timeout.tv_sec = 5;
        timeout.tv_usec = 0;

        result = select(0, &readCopy, 0, &exceptCopy, &timeout);
        // select 함수에 의해서 바뀐 것이 있다면
        if (result > 0)
        {
            // 예외가 발생했다면 -> MSG_OOB가 전달됐다면
            if (FD_ISSET(hRecvSock, &exceptCopy))
            {
                strLen = recv(hRecvSock, buf, BUF_SIZE - 1, MSG_OOB);
                buf[strLen] = '\0';
                printf("Urgent Message: %s\n", buf);
            }

            // 그 이외의 상황 -> 읽어들일 데이터가 있는 소켓이라면
            if (FD_ISSET(hRecvSock, &readCopy))
            {
                strLen = recv(hRecvSock, buf, BUF_SIZE - 1, 0);
                // EOF -> 클라이언트의 접속 종료
                if (strLen == 0)
                {
                    break;
                    closesocket(hRecvSock);
                }
                else // 그게 아니라면
                {
                    buf[strLen] = '\0';
                    puts(buf);
                }
            }
        }
    }

    closesocket(hAcptSock);
    WSACleanup();
    return 0;
}


void ErrorHandling(const char* err_msg)
{
    fputs(err_msg, stderr);
    fputc('\n', stderr);
    exit(1);
}