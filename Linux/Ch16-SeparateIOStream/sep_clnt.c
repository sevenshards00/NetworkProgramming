/*
* TCP/IP Socket Programming - 입출력 스트림의 분리에 대한 나머지 이야기
* 파일명: sep_clnt.c
* 파일 버전: 0.1
* 작성자: Sevenshards
* 작성 일자: 2023-12-27
* 이전 버전 작성 일자:
* 버전 내용: 입출력 스트림의 분리 이후 half-close에 대한 문제 - Client
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
	char buf[BUF_SIZE];
	struct sockaddr_in serv_addr;
	// 표준 입출력 함수 사용을 위한 FILE* 변수 선언
	FILE* readfp;
	FILE* writefp;
	
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
	if(inet_pton(AF_INET, argv[1], &serv_addr.sin_addr) == -1)
		error_handling("failed init address error");
	serv_addr.sin_port = htons(atoi(argv[2]));
	
	// 서버로 연결 요청(connect), 연결 요청이 성공하면 클라이언트 소켓에 주소가 할당된다.
	if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
		error_handling("connect() error");
	else
		puts("Connected............");
	
	// fdopen 함수 - file descriptor -> FILE*
	readfp = fdopen(sock, "r");
	writefp = fdopen(sock, "w");
	while(1)
	{
		if(fgets(buf, sizeof(buf), readfp) == NULL)
			break;
		
		fputs(buf, stdout);
		fflush(writefp);
	}

    // Client에서 보내는 마지막 메세지.
    // half-close가 제대로 되었다면 받을 수 있지만, 받지 못한다.
    fputs("From client: Thank you!\n", writefp);
	fclose(writefp);
	fclose(readfp);
	return 0;
}

void error_handling(const char* err_msg)
{
	fputs(err_msg, stderr);
	fputc('\n', stderr);
	exit(1);
}
