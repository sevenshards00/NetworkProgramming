/*
* TCP/IP Socket Programming - 멀티쓰레드 기반의 서버 구현
* 파일명: chat_client.c
* 파일 버전: 0.1
* 작성자: Sevenshards
* 작성 일자: 2023-12-27
* 이전 버전 작성 일자:
* 버전 내용: 멀티쓰레드 기반 다중접속 서버 구현 - Client
* 이전 버전 내용:
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

#define BUF_SIZE 100
#define IPV4_SIZE 32
#define NAME_SIZE 20

void* send_msg(void* arg); // 송신용 쓰레드의 main이 될 함수
void* recv_msg(void* arg); // 수신용 쓰레드의 main이 될 함수
void error_handling(const char* err_msg);

char name[NAME_SIZE]="[DEFAULT]";
char msg[BUF_SIZE];

int main(int argc, char* argv[])
{
    int sock;
    struct sockaddr_in serv_addr;
    // 2개의 쓰레드를 사용한다.
    // 송신용 쓰레드, 수신용 쓰레드.
    pthread_t send_thread, recv_thread;
    void* thread_return;
    if(argc != 4)
    {
        printf("Usage: %s <IP> <Port> <Nickname>\n", argv[0]);
        exit(1);
    }

    // Linux는 sprintf_s 없음
    //sprintf_s(name, sizeof(name), "[%s]", argv[3]);
    // name의 문자열을 입력받은 값으로 대체
    // sprintf는 포맷에 맞춰서 문자열을 저장
    sprintf(name, "[%s]", argv[3]);
    sock = socket(PF_INET, SOCK_STREAM, 0);

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    if(inet_pton(AF_INET, argv[1], &serv_addr.sin_addr) == -1)
        error_handling("failed init address error");
    serv_addr.sin_port = htons(atoi(argv[2]));

    if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
        error_handling("connect() error");
    
    // connect까지 완료가 되었다면 쓰레드를 생성
    pthread_create(&send_thread, NULL, send_msg, (void*)&sock);
    pthread_create(&recv_thread, NULL, recv_msg, (void*)&sock);
    // pthread_join 함수를 호출하여 쓰레드가 종료될 때까지 블로킹.
    pthread_join(send_thread, &thread_return);
    pthread_join(recv_thread, &thread_return);
    close(sock);
    return 0;
}

// 송신용 쓰레드의 main이 될 함수
void* send_msg(void* arg)
{
    int sock = *((int*)arg);
    char name_msg[NAME_SIZE + BUF_SIZE];
    while(1)
    {
        fgets(msg, BUF_SIZE, stdin);
        if(!strcmp(msg, "q\n") || !strcmp(msg, "Q\n"))
        {
            close(sock);
            exit(0);
        }
        // Linux는 sprintf_s 없음
        //sprintf_s(name_msg, sizeof(name_msg), "%s %s", name, msg);
        // "[Name] 입력한 문자열"
        sprintf(name_msg, "%s %s", name, msg);
        
        write(sock, name_msg, strlen(name_msg));
    }
    return NULL;
}

// 수신용 쓰레드의 main이 될 함수
void* recv_msg(void* arg)
{
    int sock = *((int*)arg);
    char name_msg[NAME_SIZE + BUF_SIZE];
    int str_len;
    while(1)
    {
        str_len = read(sock, name_msg, NAME_SIZE+BUF_SIZE);
        if(str_len == -1)
            return (void*)-1;
        name_msg[str_len] = '\0';
        fputs(name_msg, stdout);
    }
    return NULL;
}

void error_handling(const char* err_msg)
{
	fputs(err_msg, stderr);
	fputc('\n', stderr);
	exit(1);
}