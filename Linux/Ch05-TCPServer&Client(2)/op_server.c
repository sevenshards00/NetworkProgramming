/*
* TCP/IP Socket Programming - TCP 기반 서버/클라이언트 (2)
* 파일명: op_client.c
* 파일 버전: 0.1
* 작성자: Sevenshards
* 작성 일자: 2023-12-18
* 이전 버전 작성 일자:
* 버전 내용: TCP 기반 계산기 Server
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
int calculate(int opnum, int opnds[], char operator);

int main(int argc, char* argv[])
{
	int serv_sock, clnt_sock; // 서버 소켓, 클라이언트 소켓
	char opinfo[BUF_SIZE]; // 클라이언트로부터 들어오는 데이터를 담을 버퍼
	int result, opnd_cnt, i; // 계산 결과, 피연산자 갯수, 반복문에 사용될 변수 i
	int recv_cnt, recv_len; // 클라이언트에서 받은 문자열의 길이와 실제 받아들인 바이트의 수를 카운
	
	struct sockaddr_in serv_addr, clnt_addr; // 서버와 클라이언트의 주소를 담을 구조체, IPv4 기준
	socklen_t clnt_addr_sz; // 클라이언트의 주소 값의 크기
	
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
	
	for(i = 0; i < 5; i++)
	{	
		opnd_cnt = 0; // 피연산자의 수를 0으로 카운트
		// accept를 하여 연결을 수락하는 과정
		clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_sz);
		if(clnt_sock == -1)
			error_handling("accept() error");
		else
			printf("Connected Client %d\n", i+1);
		
		// 이번에는 서버에서 에코 클라이언트처럼 읽어들인다.
		// 클라이언트에서 얼마나 데이터를 보낼지 모르기 때문
		// 마찬가지로 클라이언트에서 보내는 데이터가 한번에 다 오지 않을 것을 생각해야 한다.
		read(clnt_sock, &opnd_cnt, 1); // 첫 바이트만 읽어들인다. 여기서 피연산자 갯수 정보를 가져온다. (Parsing)
		recv_len = 0; // 클라이언트에서 보낸 데이터의 길이를 저장하기 위해 사용한 recv_len
		// 클라이언트에 보낸 데이터의 총 길이는 (opnd_cnt * OPSZ) + 2다.
		// 앞에서 1바이트는 읽었으므로 여기서는 (opnc_cnt * OPSZ) + 1까지 읽어들이면 다 읽은 것.
		while(recv_len < (opnd_cnt * OPSZ + 1))
		{
			recv_cnt = read(clnt_sock, &opinfo[recv_len], BUF_SIZE - 1);
			recv_len += recv_cnt;
		}
		// opinfo의 마지막 인덱스에는 '\0'이 들어가있다.
		// (마지막 인덱스 - 1) 한 위치에는 연산자가 들어있으므로 이를 인자 전달.
		result = calculate(opnd_cnt, (int*)opinfo, opinfo[recv_len-1]);
		// 결과를 클라이언트에 전송, 보낼 때에는 char형 포인터로 캐스팅.
		write(clnt_sock, (char*)&result, sizeof(result));
			
		// 연결이 종료되었으므로 클라이언트 소켓을 닫는다.
		close(clnt_sock);
	}
	// 마지막으로 서버의 소켓까지 닫으면서 종료
	close(serv_sock);
	return 0;
}

int calculate(int opnum, int opnds[], char operator)
{
	int result = opnds[0], i;
	switch(operator)
	{
	case '+':
		for(i = 1; i < opnum; i++)
			result += opnds[i];
		break;
	case '-':
		for(i = 1; i < opnum; i++)
			result -= opnds[i];
		break;
	case '*':
		for(i = 1; i < opnum; i++)
			result *= opnds[i];
		break;
	}
	return result;
}

void error_handling(const char* err_msg)
{
	fputs(err_msg, stderr);
	fputc('\n', stderr);
	exit(1);
}
