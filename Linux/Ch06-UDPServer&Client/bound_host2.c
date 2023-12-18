/*
* TCP/IP Socket Programming - UDP 기반 서버/클라이언트
* 파일명: bound_host2.c
* 파일 버전: 0.1
* 작성자: Sevenshards
* 작성 일자: 2023-12-18
* 이전 버전 작성 일자:
* 버전 내용: UDP 송수신 데이터의 경계를 확인하는 예제
* 이전 버전 내용:
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 30

void error_handling(const char* err_msg);

int main(int argc, char* argv[])
{
    int sock;
    char msg1[] = "Hi!";
    char msg2[] = "I'm another UDP host!";
    char msg3[] = "Good to see you";
    
    struct sockaddr_in your_addr;
    socklen_t your_addr_sz;
    int str_len, i;

    if(argc != 3)
    {
        printf("Usage: %s <IP> <Port>\n", argv[0]);
        exit(1);
    }

    sock = socket(PF_INET, SOCK_DGRAM, 0);
    if(sock == -1)
        error_handling("socket() error");
    
    memset(&your_addr, 0, sizeof(your_addr));
    your_addr.sin_family = AF_INET;
    inet_pton(AF_INET, argv[1], &your_addr.sin_addr);
    your_addr.sin_port = htons(atoi(argv[2]));

    your_addr_sz = sizeof(your_addr);
    // 메시지를 3회 전송(sendto)
    sendto(sock, msg1, sizeof(msg1), 0, (struct sockaddr*)&your_addr, your_addr_sz);
    sendto(sock, msg2, sizeof(msg2), 0, (struct sockaddr*)&your_addr, your_addr_sz);
    sendto(sock, msg3, sizeof(msg3), 0, (struct sockaddr*)&your_addr, your_addr_sz);
    close(sock);
    return 0;
}

void error_handling(const char* err_msg)
{
	fputs(err_msg, stderr);
	fputc('\n', stderr);
	exit(1);
}