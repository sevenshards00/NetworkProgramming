/*
* TCP/IP Socket Programming - I/O 멀티플렉싱(Multiplexing)
* 파일명: echo_selectserv_win.c
* 파일 버전: 0.1
* 작성자: Sevenshards
* 작성 일자: 2023-12-25
* 이전 버전 작성 일자:
* 버전 내용: Windows 기반 select 함수를 이용한 멀티플렉싱 서버 구현
* 이전 버전 내용:
*/

#pragma comment (lib, "ws2_32.lib")

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

#define BUF_SIZE 1024
void ErrorHandling(const char* err_msg);

int main(int argc, char* argv[])
{
    WSADATA wsaData;
    SOCKET hServSock, hClntSock; // 서버, 클라이언트 소켓 파일 디스크립터
    SOCKADDR_IN servAddr, clntAddr; // 서버, 클라이언트의 주소 정보를 담을 구조체
    TIMEVAL timeout; // 타임아웃 설정을 위한 구조체
    fd_set reads, cpyReads; // select 함수에 사용될 파일 디스크립터 Set

    socklen_t addrSz;
    int str_len, fdNum, i;
    char buf[BUF_SIZE];

    if (argc != 2)
    {
        printf("Usage: %s <Port>\n", argv[0]);
        exit(1);
    }

    // WSAStartup
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        ErrorHandling("WSAStartup() error");

    // 소켓 생성
    hServSock = socket(PF_INET, SOCK_STREAM, 0);
    if (hServSock == INVALID_SOCKET)
        ErrorHandling("socket() error");

    // 소켓에 주소를 바인딩하기 위해 주소 정보 등록
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(atoi(argv[1]));

    // 소켓에 주소 바인딩(bind) 및 통신 대기(listen)
    if (bind(hServSock, (SOCKADDR*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR)
        ErrorHandling("bind() error");
    if (listen(hServSock, 5) == SOCKET_ERROR)
        ErrorHandling("listen() error");

    // fd_set reads의 모든 비트 0으로 초기화
    FD_ZERO(&reads);
    // fd_set reads에 서버 소켓 핸들 등록 -> 서버 소켓에 데이터가 수신됐다는 것은 연결 요청이 있었다고 볼 수 있음
    // 리눅스와의 차이점은 파일 디스크립터가 아닌 핸들이므로 핸들값이 어떻게 생성될지 모름
    // 그래서 배열에 소켓의 핸들값을 저장하게 되고 fd_count라는 멤버를 통해 인덱스로 접근 가능
    FD_SET(hServSock, &reads);

    while (1)
    {
        // fd_set reads의 원본 유지를 위해 cpyReads에 값을 복사
        cpyReads = reads;
        // 타임 아웃 설정, 반드시 반복문 내에서 계속 갱신되어야 한다.
        timeout.tv_sec = 5;
        timeout.tv_usec = 5000;

        // select 함수를 호출하는 부분
        // 맨 첫번째 인자인 nfds는 의미가 없는, 단순한 호환을 위해 사용되므로 0을 넣는다.
        if ((fdNum = select(0, &cpyReads, 0, 0, &timeout)) == SOCKET_ERROR)
            ErrorHandling("select() error");
        
        if (fdNum == 0)
            continue;

        // 변화가 생긴 소켓 핸들 확인을 위한 반복문 수행.
        for (i = 0; i < reads.fd_count; i++)
        {
            // FD_ISSET을 통해 수신된 데이터가 있는지 확인한다.
            if (FD_ISSET(reads.fd_array[i], &cpyReads))
            {
                // 해당 파일 디스크립터가 서버 소켓이라면 -> 연결 요청이 있었다는 것을 의미
                if (reads.fd_array[i] == hServSock)
                {
                    // 클라이언트 소켓을 생성하고 연결 요청에 대해 수락하는 과정
                    addrSz = sizeof(clntAddr);
                    hClntSock = accept(hServSock, (SOCKADDR*)&clntAddr, &addrSz);
                    if (hClntSock == SOCKET_ERROR)
                        continue;
                    else
                    {
                        puts("New Client Connected....");
                        // 새로 생성된 클라이언트와 연결된 소켓(핸들)을 fd_set reads에 추가로 등록한다
                        FD_SET(hClntSock, &reads);
                        printf("Connected Client: %d\n", hClntSock);
                    }
                }
                else // 전송된 메세지를 읽어들이는 부분
                {
                    // 상태 변화가 발생한 소켓이 서버 소켓이 아닐 경우에 실행.
                    // 다시 말해서 수신할 데이터가 있는 경우에 수행하는 구문
                    str_len = recv(reads.fd_array[i], buf, BUF_SIZE - 1, 0);
                    // 수신한 데이터가 EOF라면 소켓을 종료하고, fd_set 변수인 reads에서 해당 소켓을 삭제해야 한다.
                    if (str_len == 0)
                    {
                        FD_CLR(reads.fd_array[i], &reads); // fd_set reads에서 해당 소켓에 대한 정보를 삭제
                        // FD_CLR에서 클라이언트 소켓에 대한 정보를 삭제했기 때문에 reads에서는 클라이언트 소켓에 대한 정보가 없음.
                        // 그래서 사본을 통해 클라이언트와 연결된 소켓(핸들)을 해제
                        closesocket(cpyReads.fd_array[i]);                         
                        printf("Closed Client: %d\n", cpyReads.fd_array[i]);
                    }
                    else // 수신된 데이터가 있는 상황
                        send(reads.fd_array[i], buf, str_len, 0); // 에코 서버와 같이 동작
                }
            }
        }
    }
    closesocket(hServSock);
    return 0;
}

void ErrorHandling(const char* err_msg)
{
    fputs(err_msg, stderr);
    fputc('\n', stderr);
    exit(1);
}