/*
* TCP/IP Socket Programming - 다양한 입출력 함수들
* 파일명: peek_send.c
* 파일 버전: 0.1
* 작성자: Sevenshards
* 작성 일자: 2023-12-26
* 이전 버전 작성 일자:
* 버전 내용: MSG_PEEK, MSG_DONTWAIT 옵션을 이용한 입력 버퍼 검사 예제 - send
* 이전 버전 내용:
*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

void error_handling(const char* err_msg);

int main(int argc, char* argv[])
{
    int sock;
    struct sockaddr_in recv_addr;

    if(argc != 3)
    {
        printf("Usage: %s <IP> <Port>", argv[0]);
        exit(1);
    }

    sock = socket(PF_INET, SOCK_STREAM, 0);
    memset(&recv_addr, 0, sizeof(recv_addr));
    recv_addr.sin_family = AF_INET;
    if(inet_pton(AF_INET, argv[1], &recv_addr.sin_addr) == -1)
        error_handling("failed init address error");
    recv_addr.sin_port = htons(atoi(argv[2]));

    if(connect(sock, (struct sockaddr*)&recv_addr, sizeof(recv_addr)) == -1)
        error_handling("connect() error");

    write(sock, "123", strlen("123"));
    close(sock);
    return 0;
}

void error_handling(const char* err_msg)
{
	fputs(err_msg, stderr);
	fputc('\n', stderr);
	exit(1);
}