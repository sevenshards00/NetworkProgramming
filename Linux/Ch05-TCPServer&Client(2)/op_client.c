/*
* TCP/IP Socket Programming - TCP 기반 서버/클라이언트 (2)
* 파일명: op_client.c
* 파일 버전: 0.1
* 작성자: Sevenshards
* 작성 일자: 2023-12-18
* 이전 버전 작성 일자:
* 버전 내용: TCP 기반 계산기 Client
* 이전 버전 내용:
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 1024
#define RLT_SIZE 4 // 연산 결과의 바이트 수 매크로로 지정
#define OPSZ 4 // 피연산자의 바이트 수 매크로로 지정
void error_handling(const char* err_msg);

int main(int argc, char* argv[])
{
	int sock;
	char opmsg[BUF_SIZE]; // 여기에 입력받을 피연산자의 수 연산자, 피연산자 정보를 모두 담는다
	int result, opnd_cnt, i;
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
	
	fputs("Operand Count: ", stdout);
	scanf("%d", &opnd_cnt); // 총 입력 받을 피연산자의 갯수
	opmsg[0] = (char)opnd_cnt; // 피연산자의 갯수 정보는 1바이트 정수 형태로 전달하기로 하였으므로 char형으로 형변환
	
	// 피연산자의 갯수만큼 반복문을 통해 입력
	for(i = 0; i < opnd_cnt; i++)
	{
		printf("Operand %d: ", i + 1);
		// 이 부분이 포인트다
		// char형 배열(char*)에 int를 저장해야 되는 상황이다.
		// 그래서 강제로 int*로 형을 변환하여 int형 데이터를 저장하는 상황
		// 배열에 int형 데이터는 4바이트씩 저장이 되므로 실제로는 (i*4)+1의 인덱스마다 저장하고 있는 것
		// 맨 첫 바이트에는 피연산자의 갯수, 그 이후로는 피연산자들이 이어서 저장된다.
		// 어떻게 보면 극한의 꼼수를 쓴 느낌이다.
		scanf("%d", (int*)&opmsg[i*OPSZ+1]);
	}
	fgetc(stdin); // 마지막에 입력되어 남아있는 개행문자('\n')를 제거
	fputs("Operator: ", stdout);
	// 마찬가지로 여기도 꼼수를 쓴 부분
	// (피연산자의 갯수)*4+1을 하여 마지막 인덱스에 연산자 저장한다.
	scanf("%c", &opmsg[opnd_cnt*OPSZ+1]);
	// 왜 보내는 버퍼의 사이즈가 이렇게 계산되었는가는 코드를 천천히 따라가보면 바로 알 수 있다.
	// 맨 처음의 1은 피연산자의 갯수를 입력 받기 위한 1바이트
	// 그리고 피연산자의 갯수만큼 int 자료형이 저장되어 있으므로 opnd_cnt * OPSZ
	// 마지막은 연산자의 1바이트가 들어간다.
	write(sock, opmsg, opnd_cnt * OPSZ + 2);
	// 여기서는 굳이 반복문을 돌려가면서 read를 하지 않는 이유는 반환받을 데이터의 크기를 정해놓았기 때문이다.
	read(sock, &result, RLT_SIZE);
	
	printf("Operation result: %d\n", result);
	close(sock);
	return 0;
}

void error_handling(const char* err_msg)
{
	fputs(err_msg, stderr);
	fputc('\n', stderr);
	exit(1);
}
