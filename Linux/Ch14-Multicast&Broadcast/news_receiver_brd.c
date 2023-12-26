/*
* TCP/IP Socket Programming - 멀티캐스트 & 브로드캐스트
* 파일명: news_receiver_brd.c
* 파일 버전: 0.1
* 작성자: Sevenshards
* 작성 일자: 2023-12-26
* 이전 버전 작성 일자:
* 버전 내용: 브로드캐스트 예제 - receiver
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
    int recv_sock;
    int str_len;
    char buf[BUF_SIZE];
    struct sockaddr_in addr;

    if(argc != 2)
    {
        printf("Usage: %s <Port>\n", argv[0]);
        exit(1);
    }

    recv_sock = socket(PF_INET, SOCK_DGRAM, 0);
    if(recv_sock == -1)
        error_handling("socket() error");

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(atoi(argv[1]));

    if(bind(recv_sock, (struct sockaddr*)&addr, sizeof(addr)) == -1)
        error_handling("bind() error");

    while(1)
    {
        str_len = recvfrom(recv_sock, buf, BUF_SIZE - 1, 0, NULL, 0);
        if(str_len < 0)
            break;
        buf[str_len] = '\0';
        fputs(buf, stdout);
    }
    close(recv_sock);
    return 0;
}

void error_handling(const char* err_msg)
{
	fputs(err_msg, stderr);
	fputc('\n', stderr);
	exit(1);
}