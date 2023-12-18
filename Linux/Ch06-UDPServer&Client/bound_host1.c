/*
* TCP/IP Socket Programming - UDP 기반 서버/클라이언트
* 파일명: bound_host1.c
* 파일 버전: 0.1
* 작성자: Sevenshards
* 작성 일자: 2023-12-18
* 이전 버전 작성 일자:
* 버전 내용: UDP 송수신 데이터의 경계를 확인하는 예제
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
    int sock;
    char message[BUF_SIZE];
    struct sockaddr_in my_addr, your_addr;
    socklen_t addr_sz;
    int str_len, i;

    if(argc != 2)
    {
        printf("Usage: %s <Port>\n", argv[0]);
        exit(1);
    }

    sock = socket(PF_INET, SOCK_DGRAM, 0);
    if(sock == -1)
        error_handling("socket() error");
    
    memset(&my_addr, 0, sizeof(my_addr));
    my_addr.sin_family = AF_INET;
    my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    my_addr.sin_port = htons(atoi(argv[1]));

    if(bind(sock, (struct sockaddr*)&my_addr, sizeof(my_addr)))
        error_handling("bind() error");

    // TCP와 다르게 UDP는 입출력 함수의 호출 횟수가 일치해야 한다.
    // 클라이언트에서 데이터를 3회 전송했다면, 서버에서는 데이터를 3회 받아야 한다.
    for(i = 0; i < 3; i++)
    {
        sleep(5); // 5초 동안 프로세스를 지연시킨다.
        // 5초의 지연 시간을 두는 이유는 이미 Client쪽에서는 3번의 데이터를 보냈다. (sendto)
        // 그리고 Server쪽에서는 3회의 데이터를 모두 받아들였으므로 TCP라면 한 번에 출력하는 것이 가능하다.
        // 하지만 UDP에서는 보낸 횟수만큼 받아야한다. (recvfrom)
        addr_sz = sizeof(your_addr);
        str_len = recvfrom(sock, message, BUF_SIZE, 0, (struct sockaddr*)&your_addr, &addr_sz);
        printf("Message %d: %s\n", i+1, message);
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