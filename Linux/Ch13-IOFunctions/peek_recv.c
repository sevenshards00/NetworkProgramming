/*
* TCP/IP Socket Programming - 다양한 입출력 함수들
* 파일명: peek_recv.c
* 파일 버전: 0.1
* 작성자: Sevenshards
* 작성 일자: 2023-12-26
* 이전 버전 작성 일자:
* 버전 내용: MSG_PEEK, MSG_DONTWAIT 옵션을 이용한 입력 버퍼 검사 예제 - recv
* 이전 버전 내용:
*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define BUF_SIZE 30
void error_handling(const char* err_msg);

int main(int argc, char* argv[])
{
    int acpt_sock, recv_sock;
    struct sockaddr_in acpt_addr, recv_addr;
    int str_len, state;
    socklen_t recv_addr_sz;
    char buf[BUF_SIZE];

    if(argc != 2)
    {
        printf("Usage: %s <Port>", argv[0]);
        exit(1);
    }


    acpt_sock = socket(PF_INET, SOCK_STREAM, 0);
    memset(&acpt_addr, 0, sizeof(acpt_addr));
    acpt_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    acpt_addr.sin_port = htons(atoi(argv[1]));

    if(bind(acpt_sock, (struct sockaddr*)&acpt_addr, sizeof(acpt_addr)) == -1)
        error_handling("bind() error");

    if(listen(acpt_sock, 5) == -1)
        error_handling("listen() error");
    
    recv_addr_sz = sizeof(recv_addr);
    recv_sock = accept(acpt_sock, (struct sockaddr*)&recv_addr, &recv_addr_sz);
    if(recv_sock == -1)
        error_handling("accept() error");
    
    while(1)
    {
        // recv의 플래그로 MSG_PEEK와 MSG_DONTWAIT을 사용
        // Stack이나 Queue와 마찬가지로 입력되는 버퍼의 데이터를 확인하기만 하고 비우지는 않는다.
        // MSG_DONTWAIT을 사용한 이유는 데이터가 존재하지 않아도 블로킹 상태에 두지 않기 위함.
        str_len = recv(recv_sock, buf, sizeof(buf), MSG_PEEK|MSG_DONTWAIT);
        if(str_len > 0)
            break;
    }

    buf[str_len] = '\0';
    printf("Buffering %d bytes: %s\n", str_len, buf);

    // 앞에서의 recv에 의해 입력버퍼가 비워졌어야 하나, MSG_PEEK로 인하여 데이터를 확인만 하였음
    // 입력버퍼는 비워진 상태가 아니므로 소켓에서 데이터를 진짜로 읽어들이는 부분은 사실상 여기임
    str_len = recv(recv_sock, buf, sizeof(buf) -1, 0);
    buf[str_len] = '\0';
    printf("Read Again: %s\n", buf);

    close(recv_sock);
    close(acpt_sock);
    return 0;
}

void error_handling(const char* err_msg)
{
	fputs(err_msg, stderr);
	fputc('\n', stderr);
	exit(1);
}