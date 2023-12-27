/*
* TCP/IP Socket Programming - 멀티쓰레드 기반의 서버 구현
* 파일명: chat_server.c
* 파일 버전: 0.1
* 작성자: Sevenshards
* 작성 일자: 2023-12-27
* 이전 버전 작성 일자:
* 버전 내용: 멀티쓰레드 기반 다중접속 서버 구현 - Server
* 이전 버전 내용:
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

#define BUF_SIZE 100
#define IPV4_SIZE 32
#define MAX_CLNT 256

void* handle_clnt(void* arg); // 쓰레드에서 사용될 함수, 클라이언트 관리에 사용
void send_msg(char* msg, int len); // 메시지 전송
void error_handling(const char* err_msg);

// Client의 소켓 관리를 위한 변수와 배열
int clnt_cnt = 0;
int clnt_socks[MAX_CLNT];
pthread_mutex_t mutx;

int main(int argc, char* argv[])
{
    int serv_sock, clnt_sock;
    struct sockaddr_in serv_addr, clnt_addr;
    int clnt_addr_sz;
    char clnt_addr_buf[IPV4_SIZE] = {0,};
    pthread_t t_id;

    if(argc != 2)
    {
        printf("Usage: %s <Port>\n", argv[0]);
        exit(1);
    }

    pthread_mutex_init(&mutx, NULL);
    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    if(serv_sock == -1)
        error_handling("socket() error");
    
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[1]));

    if(bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
        error_handling("bind() error");
    
    if(listen(serv_sock, 5) == -1)
        error_handling("listen() error");


    while(1)
    {
        clnt_addr_sz = sizeof(clnt_addr);
        clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_sz);
        if(clnt_sock == -1)
            error_handling("accept() error");

        // 소켓 생성 이후 연결이 생성될 때마다 clnt_cnt를 증가
        // 배열에 소켓의 파일 디스크립터를 등록
        // 임계영역으로 둔 이유 -> 전역 변수 -> 쓰레드에 의한 동시 접근 가능
        pthread_mutex_lock(&mutx);
        clnt_socks[clnt_cnt++] = clnt_sock;
        pthread_mutex_unlock(&mutx);

        pthread_create(&t_id, NULL, handle_clnt, (void*)&clnt_sock); // 쓰레드 생성
        pthread_detach(t_id); // 쓰레드가 종료되었을 경우 완전히 소멸시키기 위해 pthread_detach 함수 호출
        if(inet_ntop(AF_INET, &clnt_addr.sin_addr, clnt_addr_buf, sizeof(clnt_addr_buf)) == NULL) // 클라이언트의 접속 IP
            error_handling("inet_ntop() error");
        printf("Connected client IP: %s\n", clnt_addr_buf);
    }
    close(serv_sock);
    return 0;
}

// 쓰레드에 의해서 실행되는 함수
void* handle_clnt(void* arg)
{
    int clnt_sock = *((int*)arg);
    int str_len = 0, i;
    char msg[BUF_SIZE];

    // EOF를 받기 전까지 계속 입력받는 메시지를 전송
    while((str_len = read(clnt_sock, msg, sizeof(msg))) != 0)
        send_msg(msg, str_len);
    
    // 메시지 전송이 끝나서 클라이언트의 접속 종료 과정
    pthread_mutex_lock(&mutx);
    for(i = 0; i < clnt_cnt; i++)
    {
        // 현재 클라이언트 소켓과 배열에 등록된 클라이언트 소켓을 비교
        if(clnt_sock == clnt_socks[i])
        {
            // 접속이 종료된 클라이언트의 자리만큼 배열을 재배치
            // 쉽게 말하면 배열의 요소를 한 칸씩 앞으로 당긴다.
            // 이 부분은 연결 리스트나 다른 자료구조를 사용할 수 있다면 깔끔할 수도 있을듯.
            while(i++ < clnt_cnt -1)
                clnt_socks[i] = clnt_socks[i + 1];
            break;
        }
    }
    clnt_cnt--; // 클라이언트가 접속을 종료하였으므로 클라이언트의 Count 감소
    pthread_mutex_unlock(&mutx);
    close(clnt_sock);
    return NULL;
}

// 현재 연결된 모든 클라이언트에게 메시지를 전송
void send_msg(char* msg, int len)
{
    int i;
    pthread_mutex_lock(&mutx);
    // for문을 통해 등록된 클라이언트의 소켓에 메시지를 write한다.
    for(i = 0; i < clnt_cnt; i++)
        write(clnt_socks[i], msg, len);
    pthread_mutex_unlock(&mutx);
}

void error_handling(const char* err_msg)
{
	fputs(err_msg, stderr);
	fputc('\n', stderr);
	exit(1);
}