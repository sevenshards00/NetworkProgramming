/*
* TCP/IP Socket Programming - 소켓의 타입과 프로토콜의 설정
* 파일명: tcp_client.c
* 파일 버전: 0.1
* 작성자: Sevenshards
* 작성 일자: 2023-12-15
* 이전 버전 작성 일자:
* 버전 내용: 연결지향형 소켓(TCP 소켓) 프로그래밍 예제 - Client
* 이전 버전 내용:
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

void error_handling(char* message);

int main(int argc, char* argv[])
{
	int sock;
	struct sockaddr_in serv_addr;
	char message[30];
	int str_len;
	int idx = 0, read_len = 0;

	if(argc != 3)
	{
		printf("Usage: %s <IP> <Port>\n", argv[0]);
		exit(1);
	}

	sock = socket(PF_INET, SOCK_STREAM, 0);
	if(sock == -1)
		error_handling("socket() error");

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
	serv_addr.sin_port = htons(atoi(argv[2]));

	if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
		error_handling("connect() error");

	while(read_len = read(sock, &message[idx++], 1))
	{
		if(str_len == -1)
			error_handling("read() error!");
		
		str_len += read_len;
	}
	
	printf("Message from server: %s\n", message);
	close(sock);
	return 0;
}

void error_handling(char* message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
