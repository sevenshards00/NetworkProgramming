/*
* TCP/IP Socket Programming - UDP 기반 서버/클라이언트
* 파일명: uecho_client_win.c
* 파일 버전: 0.1
* 작성자: Sevenshards
* 작성 일자: 2023-12-18
* 이전 버전 작성 일자:
* 버전 내용: Windows 기반 UDP Echo Client - Connected UDP Socket
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
    char message[BUF_SIZE];
    int str_len;
    int addr_sz;
    SOCKADDR_IN serv_addr, from_addr;
    if (argc != 3)
    {
        printf("Usage: %s <IP> <Port>\n", argv[0]);
        exit(1);
    }

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        ErrorHandling("WSAStartup() error");
    hSock = socket(PF_INET, SOCK_DGRAM, 0); // UDP Socket
    if (hSock == INVALID_SOCKET)
        ErrorHandling("socket() error");

    ZeroMemory(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    if (inet_pton(AF_INET, argv[1], &serv_addr.sin_addr) == SOCKET_ERROR)
        ErrorHandling("failed init address error");
    serv_addr.sin_port = htons(atoi(argv[2]));

    // 클라이언트도 TCP와 마찬가지로 서버와의 연결을 위해 주소를 입력하는 것까지는 동일
    // connect의 과정이 필요 없어졌다.
    // 하지만 하나의 호스트와 오랜 시간 데이터를 송수신한다고 하면 UDP 소켓을 connected 소켓으로 만드는 것이 효율적이다.
    // connected 소켓을 만든다는 것은 보낼 대상을 고정한다는 의미가 되는 것
    // 송수신의 대상이 정해졌기 때문에 sendto, recvfrom을 대신 write, read 함수 호출이 가능하다.
    connect(hSock, (SOCKADDR*)&serv_addr, sizeof(serv_addr));
    // server측에서 listen과 accept 과정이 빠졌다.
    // 이 또한 비연결지향형이기 때문에 연결을 하는 과정이 빠진 것
    while (1)
    {
        fputs("Insert message(q to quit): ", stdout);
        fgets(message, sizeof(message), stdin);
        if (!strcmp(message, "q\n") || !strcmp(message, "Q\n"))
            break;

        // 여기서도 마찬가지로 write, read에서 sendto, recvfrom으로 바뀌었다.
        // 연결을 하지 않는 대신 recvfrom에서는 발신지의 주소를 인자로 전달하게 된다.
        // 그리고 sendto에서는 목적지의 주소를 필요로 한다.
        // sendto(sock, message, strlen(message), 0, (struct sockaddr*)&serv_addr, sizeof(serv_addr)); // sendto
        send(hSock, message, strlen(message), 0);
        /*
        addr_sz = sizeof(from_addr);
        str_len = recvfrom(sock, message, BUF_SIZE, 0, (struct sockaddr*)&from_addr, &addr_sz); // recvfrom
        */
        str_len = recv(hSock, message, sizeof(message) - 1, 0);
        message[str_len] = '\0';
        printf("Message from server: %s", message);
    }
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