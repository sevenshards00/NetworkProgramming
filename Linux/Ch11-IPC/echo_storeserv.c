/*
* TCP/IP Socket Programming - 프로세스간 통신(Inter Process Communication)
* 파일명: echo_storeserv.c
* 파일 버전: 0.1
* 작성자: Sevenshards
* 작성 일자: 2023-12-21
* 이전 버전 작성 일자:
* 버전 내용: 파이프(Pipe) 기반 프로세스간 통신 예제 - 메시지를 저장하는 에코 서버
* 이전 버전 내용:
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 100
void error_handling(const char* err_msg);
void read_childproc(int sig);

int main(int argc, char* argv[])
{
    int serv_sock, clnt_sock;
    struct sockaddr_in serv_addr, clnt_addr;
    int fds[2]; // pipe로 사용될 파일 디스크립터

    pid_t pid;
    struct sigaction act;
    socklen_t addr_sz;
    int str_len, state;
    char buf[BUF_SIZE];
    if(argc != 2)
    {
        printf("Usage: %s <Port>\n", argv[0]);
        exit(1);
    }

    // 좀비 프로세스 생성을 막기 위한 시그널 핸들링
    act.sa_handler = read_childproc;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    state = sigaction(SIGCHLD, &act, 0);

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

    pipe(fds); // 파이프 생성
    pid = fork(); // 파이프를 통해 메시지를 복사하여 파일에 저장할 자식 프로세스 생성
    if(pid == 0)
    {
        FILE* fp = fopen("echomsg.txt", "wt");
        char msgbuf[BUF_SIZE];
        int i, len;

        for(i = 0; i < 10; i++)
        {
            len = read(fds[0], msgbuf, BUF_SIZE);
            fwrite((void*)msgbuf, 1, len, fp);
        }
        fclose(fp);
        return 0;
    }

    while(1)
    {
        addr_sz = sizeof(clnt_addr);
        clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &addr_sz);
        
        // accept 이후 fork()를 호출.
        // accept 이후에 프로세스가 생성된 것이므로 부모와 자식 모두 파일 디스크립터를 갖게 된다.

        if(clnt_sock == -1)
            continue;
        else
            puts("New Client Connected....");
        
        
        pid = fork(); // 클라리언트와 통신할 자식 프로세스 생성
        if(pid == -1) // 프로세스 생성을 실패했을 경우
        {
            close(clnt_sock);
            continue;
        }
        
        if(pid == 0) // 자식 프로세스가 실행되는 영역
        {
            close(serv_sock); // 자식 프로세스에도 서버 소켓의 파일 디스크립터가 복사되므로 자식 프로세스에서는 닫아야 한다
            // Windows에서 커널 오브젝트와 핸들의 개념을 이야기했던 개념이 여기서도 동일하다.
            // Windows에서 소켓은 커널 오브젝트로 OS가 소유하게 된다.
            // 다만 핸들은 프로세스가 생성되면서 복사가 될 수 있다.
            // Linux라고 다를 것은 없다. Windows의 핸들이 여기서는 파일 디스크립터가 된다.
            // 여기서도 소켓은 OS의 소유이므로 복사가 되는 것이 아니다.
            // 소켓을 제어할 수 있는 핸들, 즉 파일 디스크립터가 복사되는 것.
            // Windows식으로 이해한다면 Usage Count를 1 줄여줬다고 보는 것이 맞다고 본다.
            while((str_len = read(clnt_sock, buf, BUF_SIZE)) !=0)
            {
                write(clnt_sock, buf, str_len);
                write(fds[1], buf, str_len);
            }
                

            close(clnt_sock);
            puts("Client disconnected....");
            return 0;
        }
        else
            close(clnt_sock); // 서버는 자식 프로세스가 클라이언트 소켓을 복사하게 되므로 자신이 갖고 있는 소켓은 소멸해도 됨
    }
    close(serv_sock);
    return 0;
}

void read_childproc(int sig)
{
    pid_t pid;
    int status;
    pid = waitpid(-1, &status, WNOHANG);
    printf("removed proc id: %d\n", pid);
}

void error_handling(const char* err_msg)
{
	fputs(err_msg, stderr);
	fputc('\n', stderr);
	exit(1);
}