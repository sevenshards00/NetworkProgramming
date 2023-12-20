/*
* TCP/IP Socket Programming - 소켓의 연결 종료
* 파일명: file_client.c
* 파일 버전: 0.1
* 작성자: Sevenshards
* 작성 일자: 2023-12-20
* 이전 버전 작성 일자:
* 버전 내용: 파일 전송 기반 Half-close 예시 - Client
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
    int sd;
    FILE* fp;

    char buf[BUF_SIZE];
    int read_cnt;
    struct sockaddr_in serv_addr;
    if(argc != 3)
    {
        printf("Usage: %s <IP> <Port>\n", argv[0]);
        exit(1);
    }

    fp = fopen("receive.dat", "wb"); // 서버에서 보내는 데이터를 담기 위해 파일을 생성.
    sd = socket(PF_INET, SOCK_STREAM, 0);
    if(sd == -1)
        error_handling("socket() error");

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    if(inet_pton(AF_INET, argv[1], &serv_addr.sin_addr) == -1)
        error_handling("fail init address");
    serv_addr.sin_port = htons(atoi(argv[2]));

    if(connect(sd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
        error_handling("connect error");
    else
        puts("Connected.......");

    // 서버에서 데이터를 얼마나 전송할지 모른다.
    // 그래서 read 함수를 호출하고 반환하는 값이 0이면 더 이상 서버에서 보내는 데이터가 없는 것
    // 다시 말해서 서버에서 보내는 데이터가 없을 때까지 계속 read를 한다
    // 현재 코드를 기준으로는 EOF가 전송될 때까지 읽는 것
    while((read_cnt = read(sd, buf, BUF_SIZE)) != 0)
        fwrite((void*)buf, 1, read_cnt, fp); // 서버에서 보내는 데이터를 EOF가 전송될 때까지 파일에 저장.

    fputs("Received file data\n", stdout);
    write(sd, "Thank you", 10); // 마지막으로 연결을 종료하기 전 서버에 메시지를 보낸다.
    fclose(fp); // 파일 스트림을 닫고
    close(sd); // 소켓 연결 종료
    return 0;
}

void error_handling(const char* err_msg)
{
	fputs(err_msg, stderr);
	fputc('\n', stderr);
	exit(1);
}