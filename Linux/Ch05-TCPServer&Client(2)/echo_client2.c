/*
* TCP/IP Socket Programming - TCP 기반 서버/클라이언트 (2)
* 파일명: echo_client2.c
* 파일 버전: 0.1
* 작성자: Sevenshards
* 작성 일자: 2023-12-18
* 이전 버전 작성 일자:
* 버전 내용: TCP 기반 Iterative Echo Client - 문제 있는 부분 수정
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
	int str_len; // 문자열의 길이
	int recv_len, recv_cnt; // 서버에서 받은 문자열의 길이와 실제 받아들인 바이트 수 카운트
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
		// write(sock, message, strlen(message));
		// str_len = read(sock, message, BUF_SIZE-1);
		// 위의 코드를 다음과 같이 수정
		
		// 서버에서 보내는 데이터가 한번에 다 오지 않을 수도 있음을 고려해야 함
		// 현재 echo 클라이언트 기준으로는 내가 보낸 데이터의 크기를 내가 알고 있는 상황임
		// 그래서 내가 보내는 문자열의 길이를 write하면서 전체 길이를 저장
		str_len = write(sock, message, strlen(message));
		
		recv_len = 0; // 서버에서 보낸 데이터의 길이를 저장하기 위해 사용한 recv_len
		while(recv_len < str_len) // 서버에서 보낸 데이터의 길이가 내가 보낸 데이터의 길이와 같아야 반복문 종료
		{
			recv_cnt = read(sock, &message[recv_len], BUF_SIZE - 1); // 서버에서 보낸 데이터를 읽어서 recv_cnt에 대입
			if(recv_cnt == -1)
				error_handling("read() error");
			recv_len += recv_cnt; // 실제로 서버에서 읽어들인 바이트의 수만큼 recv_cnt를 recv_len에 더한다
		}
		message[recv_len] ='\0'; // 문자열로 구분하기 위해 마지막에 널('\0')을 저장
		
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
