/*
* TCP/IP Socket Programming - 소켓의 다양한 옵션
* 파일명: buf_win.c
* 파일 버전: 0.1
* 작성자: Sevenshards
* 작성 일자: 2023-12-20
* 이전 버전 작성 일자:
* 버전 내용: Windows 기반 getsockopt, setsockopt 함수 사용 예제
* 이전 버전 내용:
*/

#pragma comment (lib, "ws2_32.lib")

#include <stdio.h>
#include <stdlib.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

void ErrorHandling(const char* err_msg);

int main(int argc, char* argv[])
{
    WSADATA wsaData;
    SOCKET hSock;
    int snd_buf = 1024 * 3, rcv_buf = 1024 * 3; // 입/출력 버퍼를 3바이트로 지정
    int state;
    socklen_t len;

    if(WSAStartup(MAKEWORD(2,2), &wsaData) != 0)
        ErrorHandling("WSAStartup() error");
    
    hSock = socket(PF_INET, SOCK_STREAM, 0);
    len = sizeof(snd_buf);
    
    // getsockopt
    state = getsockopt(hSock, SOL_SOCKET, SO_SNDBUF, (char*)&snd_buf, &len); // 출력 버퍼 크기 참조
    if (state)
        ErrorHandling("getsockopt() error");
    state = getsockopt(hSock, SOL_SOCKET, SO_RCVBUF, (char*)&rcv_buf, &len); // 입력 버퍼 크기 참조
    if (state)
        ErrorHandling("getsockopt() error");

    printf("Input buffer size: %d\n", rcv_buf);
    printf("Output buffer size: %d\n", snd_buf);

    //setsockopt
    state = setsockopt(hSock, SOL_SOCKET, SO_SNDBUF, (char*)&snd_buf, &len);
    if (state)
        ErrorHandling("setsockipt() error");
    state = setsockopt(hSock, SOL_SOCKET, SO_SNDBUF, (char*)&rcv_buf, &len);
    if (state)
        ErrorHandling("setsockipt() error");

    printf("Input buffer size: %d\n", rcv_buf);
    printf("Output buffer size: %d\n", snd_buf);
    
    WSACleanup();
    return 0;
}

void ErrorHandling(const char* err_msg)
{
    fputs(err_msg, stderr);
    fputc('\n', stderr);
    exit(1);
}