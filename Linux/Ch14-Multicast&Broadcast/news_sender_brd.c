/*
* TCP/IP Socket Programming - 멀티캐스트 & 브로드캐스트
* 파일명: news_sender_brd.c
* 파일 버전: 0.1
* 작성자: Sevenshards
* 작성 일자: 2023-12-26
* 이전 버전 작성 일자:
* 버전 내용: 브로드캐스트 예제 - sender
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
    int send_sock;
    struct sockaddr_in broad_addr;
    FILE* fp;
    char buf[BUF_SIZE];
    int so_brd = 1;

    if(argc != 3)
    {
        printf("Usage: %s <Broadcast IP> <Port>\n", argv[0]);
        exit(1);
    }

    send_sock = socket(PF_INET, SOCK_DGRAM, 0);
    if(send_sock == -1)
        error_handling("socket() error");

    memset(&broad_addr, 0, sizeof(broad_addr));
    broad_addr.sin_family = AF_INET;
    if(inet_pton(AF_INET, argv[1], &broad_addr.sin_addr) == -1)
        error_handling("failed init address error");
    broad_addr.sin_port = htons(atoi(argv[2]));

    setsockopt(send_sock, SOL_SOCKET, SO_BROADCAST, (void*)&so_brd, sizeof(so_brd));
    if((fp = fopen("news.txt", "r")) == NULL)
        error_handling("fopen() error");

    while(!feof(fp))
    {
        fgets(buf, BUF_SIZE, fp);
        sendto(send_sock, buf, strlen(buf), 0, (struct sockaddr*)&broad_addr, sizeof(broad_addr));
        sleep(2);
    }

    fclose(fp);
    close(send_sock);
    return 0;
}

void error_handling(const char* err_msg)
{
	fputs(err_msg, stderr);
	fputc('\n', stderr);
	exit(1);
}