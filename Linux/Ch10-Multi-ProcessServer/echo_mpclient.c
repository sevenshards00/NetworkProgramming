/*
* TCP/IP Socket Programming - 멀티프로세스 기반의 서버 구현
* 파일명: echo_mpclient.c
* 파일 버전: 0.1
* 작성자: Sevenshards
* 작성 일자: 2023-12-20
* 이전 버전 작성 일자:
* 버전 내용: Linux 기반 멀티 프로세스 기반의 echo 클라이언트 구현 - 입출력 루틴 분할
* 이전 버전 내용:
*/

// 책에서도 나온 내용이지만, 굳이 에코 클라이언트를 입출력 루틴을 분할할 이유는 없다.
// 개념적인 차원에서 이해하고 넘어가자는 측면에서 입출력 루틴의 분할을 적용하는 것.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 30
void error_handling(const char* err_msg);
void read_routine(int sock, char* buf); // 입력 루틴
void write_routine(int sock, char* buf); // 출력 루틴

int main(int argc, char* argv[])
{
	int sock;
    pid_t pid;
	char buf[BUF_SIZE];
	struct sockaddr_in serv_addr;
	
	if(argc != 3)
	{
		printf("Usage: %s <IP> <Port>\n", argv[0]);
		exit(1);
	}
	
	// 클라이언트의 소켓 생성
	sock = socket(PF_INET, SOCK_STREAM, 0);
	if(sock == -1)
		error_handling("socket() error");
	
	// 입력받은 값으로 sockaddr_in 구조체의 값을 채움 -> 서버의 주소를 설정
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
    if(inet_pton(AF_INET, argv[1], &serv_addr.sin_addr.s_addr) == -1)
        error_handling("failed init address error");
	serv_addr.sin_port = htons(atoi(argv[2]));
	
	// 서버로 연결 요청(connect), 연결 요청이 성공하면 클라이언트 소켓에 주소가 할당된다.
	if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
		error_handling("connect() error");
	else
		puts("Connected............");

    // 서버와 연결이 된 이후 프로세스를 생성
    pid = fork();
    if(pid == 0) // 자식 프로세스에게는 출력 루틴을
        write_routine(sock, buf);
    else // 부모 프로세스에게는 입력 루틴을
        read_routine(sock, buf);
	
	close(sock);
	return 0;
}

// 데이터의 입력만 담당.
void read_routine(int sock, char* buf)
{
    while(1)
    {
        int str_len = read(sock, buf, BUF_SIZE);
        if(str_len == 0)
            return;
        
        buf[str_len] = '\0';
        printf("Message from server: %s", buf);
    }    
}

// 데이터의 출력만 담당
void write_routine(int sock, char* buf)
{
    while(1)
    {
        fgets(buf, BUF_SIZE, stdin);
        if(!strcmp(buf, "q\n") || !strcmp(buf, "Q\n"))
        {
            shutdown(sock, SHUT_WR); // 더 이상 출력을 하지 않겠다면 EOF를 서버에 전달해야 한다.
            // 그래서 shutdown 함수를 호출하고 출력 스트림을 닫는다.
            return;
        }
        write(sock, buf, strlen(buf));
    }
}

void error_handling(const char* err_msg)
{
	fputs(err_msg, stderr);
	fputc('\n', stderr);
	exit(1);
}
