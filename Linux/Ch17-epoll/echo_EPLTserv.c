/*
* TCP/IP Socket Programming - select보다 나은 epoll
* 파일명: echo_EPLTserv.c
* 파일 버전: 0.1
* 작성자: Sevenshards
* 작성 일자: 2023-12-27
* 이전 버전 작성 일자:
* 버전 내용: epoll 기반의 echo 서버 - Level Trigger
* 이전 버전 내용:
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
// epoll 기반 서버에서 사용하는 헤더파일
#include <sys/epoll.h>

#define BUF_SIZE 4 // Level Trigger 동작 방식 이해를 위한 버퍼 사이즈 크기 변경 100->4
#define EPOLL_SIZE 50
void error_handling(const char* err_msg);

int main(int argc, char* argv[])
{
    int serv_sock, clnt_sock; // 서버, 클라이언트 소켓 파일 디스크립터
    struct sockaddr_in serv_addr, clnt_addr; // 서버, 클라이언트의 주소 정보를 담을 구조체
    socklen_t addr_sz; 
    int str_len, i;
    char buf[BUF_SIZE];
    // epoll 기반 서버에서 추가로 사용할 변수
    struct epoll_event* ep_events;
    struct epoll_event event;
    int epfd, event_cnt;

    if(argc != 2)
    {
        printf("Usage: %s <Port>\n", argv[0]);
        exit(1);
    }
    
    // 소켓 생성
    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    if(serv_sock == -1)
        error_handling("socket() error");

    // 소켓에 주소를 바인딩하기 위해 주소 정보 등록
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[1]));

    // 소켓에 주소 바인딩(bind) 및 통신 대기(listen)
    if(bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
        error_handling("bind() error");
    if(listen(serv_sock, 5) == -1)
        error_handling("listen() error");
    
    epfd = epoll_create(EPOLL_SIZE); // epoll 인스턴스 생성, size는 사실 큰 의미가 없다.
    ep_events = malloc(sizeof(struct epoll_event)*EPOLL_SIZE); // 관찰할 대상이 50이라면 50개의 이벤트만큼 동적할당

    event.events = EPOLLIN; // 수신할 데이터가 존재하는 상황(이벤트) 발생 시 -> 이벤트 트리거
    event.data.fd = serv_sock; // serv_sock이라는 파일 디스크립터의 이벤트 발생 여부를 감시한다.
    epoll_ctl(epfd, EPOLL_CTL_ADD, serv_sock, &event); // epoll 인스턴스인 epfd에 serv_sock 파일 디스크립터를 등록

    while(1)
    {
        // select와 유사한 역할을 수행하는 epoll_wait
        event_cnt = epoll_wait(epfd, ep_events, EPOLL_SIZE, -1);
        if(event_cnt == -1)
        {
            puts("epoll_wait() error");
            break;
        }
        // Level Trigger의 동작 방식 확인을 위한 문자열 출력
        puts("return epoll_wait");

        // 변화가 생긴 파일 디스크립터 확인을 위한 반복문 수행.
        // epoll 인스턴스에 등록된 이벤트의 수만큼 반복
        for(i = 0; i < event_cnt; i++)
        {
            // 이벤트가 발생한 파일 디스크립터가 서버 소켓이라면 -> 연결 요청이 있었다는 것을 의미
            if(ep_events[i].data.fd == serv_sock)
            {
                // 클라이언트 소켓을 생성하고 연결 요청에 대해 수락하는 과정
                addr_sz = sizeof(clnt_addr);
                clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &addr_sz);
                if(clnt_sock == -1)
                    continue;
                else
                {
                    puts("New Client Connected....");
                    // 새로 생성된 클라이언트와 연결된 소켓(파일 디스크립터)를 epoll 인스턴스에 추가로 등록한다
                    event.events = EPOLLIN;
                    //event.events = EPOLLIN|EPOLLET; // Edge Trigger 모델의 이벤트 등록 방식을 보려면 해당 코드로 수행
                    event.data.fd = clnt_sock;
                    epoll_ctl(epfd, EPOLL_CTL_ADD, clnt_sock, &event);
                    printf("Connected Client: %d\n", clnt_sock);
                }
            }
            else
            {
                // 상태 변화가 발생한 소켓이 서버 소켓이 아닐 경우에 실행.
                // 다시 말해서 수신할 데이터가 있는 경우에 수행하는 구문
                str_len = read(ep_events[i].data.fd, buf, BUF_SIZE);
                // 수신한 데이터가 EOF라면 소켓을 종료하고, fd_set 변수인 reads에서 해당 소켓을 삭제해야 한다.
                if(str_len == 0)
                {
                    epoll_ctl(epfd, EPOLL_CTL_DEL, ep_events[i].data.fd, NULL); // epoll 인스턴스에서 해당 파일 디스크립터의 등록을 해제
                    close(ep_events[i].data.fd); // 클라이언트와 연결된 소켓(파일 디스크립터)를 해제
                    printf("Closed Client: %d\n", ep_events[i].data.fd);
                }
                else // 수신된 데이터가 있는 상황
                    write(ep_events[i].data.fd, buf, str_len); // 에코 서버와 같이 동작
            }
        }
    }
    close(serv_sock);
    close(epfd);
    return 0;
}

void error_handling(const char* err_msg)
{
	fputs(err_msg, stderr);
	fputc('\n', stderr);
	exit(1);
}