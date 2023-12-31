/*
* TCP/IP Socket Programming - TCP 기반 서버/클라이언트 (1)
* 파일명: echo_client.c
* 파일 버전: 0.1
* 작성자: Sevenshards
* 작성 일자: 2023-12-17
* 이전 버전 작성 일자:
* 버전 내용: TCP 기반 Iterative Echo Client
* 이전 버전 내용:
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 1024
void error_handling(const char* err_msg);

int main(int argc, char* argv[])
{
	int sock;
	char message[BUF_SIZE];
	int str_len;
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
	inet_aton(argv[1], &serv_addr.sin_addr);
	serv_addr.sin_port = htons(atoi(argv[2]));
	
	// 서버로 연결 요청(connect), 연결 요청이 성공하면 클라이언트 소켓에 주소가 할당된다.
	if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
		error_handling("connect() error");
	else
		puts("Connected............");
	
	while(1)
	{
		fputs("Input message(Q to quit): ", stdout);
		fgets(message, BUF_SIZE, stdin);
		
		if(!strcmp(message, "q\n") || !strcmp(message, "Q\n"))
			break;
		// 현 시점에서 문제가 되는 부분
		// TCP 통신의 특성은 데이터의 경계가 없다는 것.
		// 지금 코드는 패킷이 나눠져서 보내는 경우를 전혀 고려하지 않은 상황
		write(sock, message, strlen(message));
		str_len = read(sock, message, BUF_SIZE-1);
		// 현 시점에서 문제가 되는 부분
		message[str_len] = 0;
		printf("Message from server: %s" , message);
	}
	close(sock);
	return 0;
}

void error_handling(const char* err_msg)
{
	fputs(err_msg, stderr);
	fputc('\n', stderr);
	exit(1);
}
