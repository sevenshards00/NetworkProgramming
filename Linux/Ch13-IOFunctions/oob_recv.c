/*
* TCP/IP Socket Programming - 다양한 입출력 함수들
* 파일명: oob_recv.c
* 파일 버전: 0.1
* 작성자: Sevenshards
* 작성 일자: 2023-12-26
* 이전 버전 작성 일자:
* 버전 내용: recv 함수를 이용한 MSG_OOB(Out-Of-Band Data) 옵션
* 이전 버전 내용:
*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>

#define BUF_SIZE 30
void error_handling(const char* err_msg);
void urg_handler(int signo);

int acpt_sock;
int recv_sock;

int main(int argc, char* argv[])
{
    struct sockaddr_in recv_addr, serv_addr;
    int str_len, state;
    socklen_t serv_addr_sz;
    struct sigaction act;
    char buf[BUF_SIZE];

    if(argc != 2)
    {
        printf("Usage: %s <Port>", argv[0]);
        exit(1);
    }

    act.sa_handler = urg_handler; // signal을 사용한다
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;

    acpt_sock = socket(PF_INET, SOCK_STREAM, 0);
    memset(&recv_addr, 0, sizeof(recv_addr));
    recv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    recv_addr.sin_port = htons(atoi(argv[1]));

    if(bind(acpt_sock, (struct sockaddr*)&recv_addr, sizeof(recv_addr)) == -1)
        error_handling("bind() error");

    if(listen(acpt_sock, 5) == -1)
        error_handling("listen() error");
    
    serv_addr_sz = sizeof(serv_addr);
    recv_sock = accept(acpt_sock, (struct sockaddr*)&serv_addr, &serv_addr_sz);
    if(recv_sock == -1)
        error_handling("accept() error");
    
    // fcntl이 가지는 의미
    // 파일 디스크립터 recv_sock이 가리키는 소켓의 소유자(F_SETOWN)를 getpid 함수가 반환하는 ID의 프로세스로 변경
    // 엄밀히 따지면 소켓은 OS에 의해서 관리됨
    // 그래서 달리 말하면 위 상황은 다음과 같이 해석
    // 파일 디스크립터 recv_sock이 가리키는 소켓에 의해 발생하는 SIGURG 시그널을 처리하는 프로세스를 getpid 함수가 반환하는 ID의 프로세스로 변경
    // SIGURG 시그널을 핸들링 할 때에는 반드시 시그널을 처리할 프로세스를 지정해야 함
    fcntl(recv_sock, F_SETOWN, getpid());
    state = sigaction(SIGURG, &act, 0); // 시그널 등록, SIGURG가 발생하면 수행

    while((str_len = recv(recv_sock, buf, sizeof(buf), 0)) != 0)
    {
        if(str_len == -1)
            continue;
        buf[str_len] = 0;
        puts(buf);
    }

    close(recv_sock);
    close(acpt_sock);
    return 0;
}

void urg_handler(int signo)
{
    int str_len;
    char buf[BUF_SIZE];
    str_len = recv(recv_sock, buf, sizeof(buf) - 1, MSG_OOB);
    buf[str_len] = 0;
    printf("urg_handler called\n");
    printf("Urgent Message: %s\n", buf);
}

void error_handling(const char* err_msg)
{
	fputs(err_msg, stderr);
	fputc('\n', stderr);
	exit(1);
}