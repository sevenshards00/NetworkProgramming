/*
* TCP/IP Socket Programming - I/O 멀티플렉싱(Multiplexing)
* 파일명: echo_selectserv.c
* 파일 버전: 0.1
* 작성자: Sevenshards
* 작성 일자: 2023-12-25
* 이전 버전 작성 일자:
* 버전 내용: Linux 기반 select 함수를 이용한 멀티플렉싱 서버 구현
* 이전 버전 내용:
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h>

#define BUF_SIZE 100
void error_handling(const char* err_msg);

int main(int argc, char* argv[])
{
    int serv_sock, clnt_sock; // 서버, 클라이언트 소켓 파일 디스크립터
    struct sockaddr_in serv_addr, clnt_addr; // 서버, 클라이언트의 주소 정보를 담을 구조체
    struct timeval timeout; // 타임아웃 설정을 위한 구조체
    fd_set reads, cpy_reads; // select 함수에 사용될 파일 디스크립터 Set

    socklen_t addr_sz;
    int fd_max, str_len, fd_num, i;
    char buf[BUF_SIZE];

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

    // fd_set reads의 모든 비트 0으로 초기화
    FD_ZERO(&reads); 
    // fd_set reads에 서버 소켓 파일 디스크립터 등록 -> 서버 소켓에 데이터가 수신된 것이 있다는 것은 연결 요청이 있었다고 해석
    FD_SET(serv_sock, &reads);
    // 생성한 서버 소켓의 파일 디스크립터 값을 등록
    fd_max = serv_sock;  

    while(1)
    {
        // fd_set reads의 원본 유지를 위해 cpy_reads에 값을 복사
        cpy_reads = reads;
        // 타임 아웃 설정, 반드시 반복문 내에서 계속 갱신되어야 한다.
        timeout.tv_sec = 5;
        timeout.tv_usec = 5000;

        // select 함수를 호출하는 부분, 파일 디스크립터는 0부터 시작하므로 최대값보다 하나 더 크게 잡으면 된다.
        // 현재 시스템 기준으로는 0~2까지는 표준 입출력, 에러이고 서버 소켓이 생성되면 3이 부여됨
        if((fd_num = select(fd_max + 1, &cpy_reads, 0, 0, &timeout)) == -1)
            error_handling("select() error");
        if(fd_num == 0)
            continue;

        // 변화가 생긴 파일 디스크립터 확인을 위한 반복문 수행.
        for(i=0; i < fd_max + 1; i++)
        {
            // FD_ISSET을 통해 수신된 데이터가 있는지 확인한다.
            if(FD_ISSET(i, &cpy_reads))
            {
                // 해당 파일 디스크립터가 서버 소켓이라면 -> 연결 요청이 있었다는 것을 의미
                if(i == serv_sock)
                {
                    // 클라이언트 소켓을 생성하고 연결 요청에 대해 수락하는 과정
                    addr_sz = sizeof(clnt_addr);
                    clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &addr_sz);
                    if(clnt_sock == -1)
                        continue;
                    else
                    {
                        puts("New Client Connected....");
                        // 새로 생성된 클라이언트와 연결된 소켓(파일 디스크립터)를 fd_set reads에 추가로 등록한다
                        FD_SET(clnt_sock, &reads);
                        // 만약 클라이언트 소켓의 파일 디스크립터 번호가 더 높다면
                        if(fd_max < clnt_sock)
                            // fd_max를 갱신한다.
                            // 앞의 기준에서 서버 소켓이 3이었으므로 클라이언트 소켓은 4이후로 부여가 되었을 것.
                            fd_max = clnt_sock;
                        printf("Connected Client: %d\n", clnt_sock);
                    }
                }
                else
                {
                    // 상태 변화가 발생한 소켓이 서버 소켓이 아닐 경우에 실행.
                    // 다시 말해서 수신할 데이터가 있는 경우에 수행하는 구문
                    str_len = read(i, buf, BUF_SIZE);
                    // 수신한 데이터가 EOF라면 소켓을 종료하고, fd_set 변수인 reads에서 해당 소켓을 삭제해야 한다.
                    if(str_len == 0)
                    {
                        FD_CLR(i, &reads); // fd_set reads에서 해당 파일 디스크립터(소켓)을 삭제
                        close(i); // 클라이언트와 연결된 소켓(파일 디스크립터)를 해제
                        printf("Closed Client: %d\n", i);
                    }
                    else // 수신된 데이터가 있는 상황
                        write(i, buf, str_len); // 에코 서버와 같이 동작
                }
            }
        }
    }
    close(serv_sock);
    return 0;
}

void error_handling(const char* err_msg)
{
	fputs(err_msg, stderr);
	fputc('\n', stderr);
	exit(1);
}