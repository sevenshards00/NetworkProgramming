/*
* TCP/IP Socket Programming - 소켓의 연결 종료
* 파일명: file_server.c
* 파일 버전: 0.1
* 작성자: Sevenshards
* 작성 일자: 2023-12-20
* 이전 버전 작성 일자:
* 버전 내용: 파일 전송 기반 Half-close 예시 - Server
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
    int serv_sd, clnt_sd;
    FILE* fp;
    char buf[BUF_SIZE];
    int read_cnt, i;

    struct sockaddr_in serv_addr, clnt_addr;

    if(argc != 2)
    {
        printf("Usage: %s <Port>\n", argv[0]);
        exit(1);
    }

    fp = fopen("file_server.c", "rb"); // 클라이언트에 보낼 파일 포인터 read
    serv_sd = socket(PF_INET, SOCK_STREAM, 0);
    socklen_t clnt_addr_sz;

    if(serv_sd == -1)
        error_handling("socket() error");

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[1]));

    if(bind(serv_sd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
        error_handling("bind() error");

    if(listen(serv_sd, 5) == -1)
        error_handling("listen() error");

    clnt_addr_sz = sizeof(clnt_addr);

    for(i = 0; i < 5; i++)
    {
        clnt_sd = accept(serv_sd, (struct sockaddr*)&clnt_addr, &clnt_addr_sz);
        
        if(clnt_sd == -1)
            error_handling("accept() error");
        else
            printf("Connected Client %d", i);

        while(1)
        {
            read_cnt = fread((void*)buf, 1, BUF_SIZE, fp); // 파일을 읽어 버퍼에 저장, 읽어들인 바이트의 수를 read_cnt에 저장
            if(read_cnt < BUF_SIZE) // read_cnt가 BUF_SIZE보다 적을 경우에도 마지막으로 남은 데이터는 소켓을 통해 전송되어야 함
            {
                write(clnt_sd, buf, read_cnt); // 마지막 데이터를 전송하고
                break; // 반복문을 빠져 나온다
            }
            write(clnt_sd, buf, BUF_SIZE); // read_cntrk BUF_SIZE보다 적지 않을 경우에는 30바이트씩 전송
        }

        shutdown(clnt_sd, SHUT_WR); // Half-close를 하는 부분, 전송은 다 끝났으므로 출력 스트림만 닫는다.
        read(clnt_sd, buf, BUF_SIZE); // 출력 스트림만 닫았으므로 클라이언트에서 보내는 데이터의 수신은 가능
        printf("Message from client: %s\n", buf);

        close(clnt_sd); // 클라이언트에서 보내는 데이터까지 다 받았으므로 소켓을 종료
        
    }

    fclose(fp);
    close(serv_sd);   

    return 0;
}

void error_handling(const char* err_msg)
{
	fputs(err_msg, stderr);
	fputc('\n', stderr);
	exit(1);
}