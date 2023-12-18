/*
* TCP/IP Socket Programming - UDP 기반 서버/클라이언트
* 파일명: uecho_server.c
* 파일 버전: 0.1
* 작성자: Sevenshards
* 작성 일자: 2023-12-18
* 이전 버전 작성 일자:
* 버전 내용: UDP 기반 Echo Server
* 이전 버전 내용:
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 30

void error_handling(const char* err_msg);

int main(int argc, char* argv[])
{
	int serv_sock; // TCP에서는 소켓을 2개를 썼으나 UDP에서는 서버던 클라이언트건 하나면 됨
	char message[BUF_SIZE];
	int str_len;
	socklen_t clnt_addr_sz;
	
	struct sockaddr_in serv_addr, clnt_addr;
	if(argc != 2)
	{
		printf("Usage: %s <Port>", argv[0]);
		exit(1);
	}
	
	serv_sock = socket(PF_INET, SOCK_DGRAM, 0); // UDP Socket
	if(serv_sock == -1)
		error_handling("socket() error");
	
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(atoi(argv[1]));
	
	if(bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)))
		error_handling("bind() error");
	
	// 소켓에 주소를 binding하는 과정까지는 TCP와 동일하다.
	// 하지만 UDP는 비연결지향형이므로 listen, accept의 과정이 필요하지 않다.
	
	while(1)
	{
		clnt_addr_sz = sizeof(clnt_addr);
		// read와 write 대신 recvfrom, sendto로 바뀌었다.
		// 연결을 하지 않는 대신 recvfrom에서는 발신지의 주소를 인자로 전달하게 된다.
		// 그리고 sendto에서는 목적지의 주소를 필요로 한다.
		str_len = recvfrom(serv_sock, message, BUF_SIZE, 0, (struct sockaddr*)&clnt_addr, &clnt_addr_sz); // recvfrom
		sendto(serv_sock, message, str_len, 0, (struct sockaddr*)&clnt_addr, clnt_addr_sz); // sendto
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
