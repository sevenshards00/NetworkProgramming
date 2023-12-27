/*
* TCP/IP Socket Programming - 입출력 스트림의 분리에 대한 나머지 이야기
* 파일명: sep_serv.c
* 파일 버전: 0.1
* 작성자: Sevenshards
* 작성 일자: 2023-12-27
* 이전 버전 작성 일자:
* 버전 내용: 입출력 스트림의 분리 이후 half-close에 대한 문제 - Server
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
	int serv_sock, clnt_sock; // 서버 소켓, 클라이언트 소켓
    // 표준 입출력 함수를 사용하기 위한 파일 포인터 변수 선언
	FILE* readfp;
	FILE* writefp;

	struct sockaddr_in serv_addr, clnt_addr; // 서버와 클라이언트의 주소를 담을 구조체, IPv4 기준
	socklen_t clnt_addr_sz; // 클라이언트의 주소 값의 크기
    char buf[BUF_SIZE] = {0,};
	
	if(argc != 2)
	{
		printf("Usage: %s <Port>\n", argv[0]);
		exit(1);
	}
	
	serv_sock = socket(PF_INET, SOCK_STREAM, 0); // 서버 소켓 생성
	if(serv_sock == -1) // 소켓 생성이 안됐을 경우
		error_handling("socket() error");
	
	memset(&serv_addr, 0, sizeof(serv_addr)); // serv_addr 구조체의 메모리를 전부 0으로 초기화
	serv_addr.sin_family = AF_INET; // IPv4, IPv6라면 AF_INET6
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); // INADDR_ANY는 서버 측에서 사용. 컴퓨터의 IP 주소를 자동으로 할당
	serv_addr.sin_port = htons(atoi(argv[1])); // Port

	// 서버 소켓에 주소를 할당(bind)
	if(bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
		error_handling("bind() error");
	
	// 서버 소켓을 연결 요청 대기 상태(listening socket)로 둔다.
	// 여기서 두 번째 인자는 연결 요청 대기 큐의 크기	
	if(listen(serv_sock, 5) == -1)
		error_handling("listen() error");
	
	// 클라이언트의 주소값 크기를 대입
	clnt_addr_sz = sizeof(clnt_addr);
	
    clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_sz);
	if(clnt_sock == -1)
		error_handling("accept() error");
	
    // fdopen 함수 - file descriptor -> FILE*
	readfp = fdopen(clnt_sock, "r");
	writefp = fdopen(clnt_sock, "w");
	
    fputs("From Server: Hello Client!\n", writefp);
    fputs("I love all of the world\n", writefp);
    fputs("You are awesome!\n", writefp);
    fflush(writefp);

    // Client에게 전송할 데이터를 다 보낸 이후 half-close 시도
    // 그러나 안된다 -> 소켓이 완전히 종료되어 버린다.
    fclose(writefp);
    fgets(buf, sizeof(buf), readfp);
    fputs(buf, stdout);
	fclose(readfp);
	
	return 0;
}

void error_handling(const char* err_msg)
{
	fputs(err_msg, stderr);
	fputc('\n', stderr);
	exit(1);
}
