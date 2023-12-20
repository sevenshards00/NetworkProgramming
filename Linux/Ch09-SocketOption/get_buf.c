/*
* TCP/IP Socket Programming - 소켓의 다양한 옵션
* 파일명: get_buf.c
* 파일 버전: 0.1
* 작성자: Sevenshards
* 작성 일자: 2023-12-20
* 이전 버전 작성 일자:
* 버전 내용: Linux 기반 getsockopt 함수 사용 예제(2) - 입출력 버퍼 크기 확인
* 이전 버전 내용:
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>

void error_handling(const char* err_msg);

int main(int argc, char* argv[])
{
    int sock;
    int snd_buf, rcv_buf, state;
    socklen_t len;

    sock = socket(PF_INET, SOCK_STREAM, 0);
    len = sizeof(snd_buf);
    state = getsockopt(sock, SOL_SOCKET, SO_SNDBUF, (void*)&snd_buf, &len); // 출력 버퍼 크기 참조
    if(state)
        error_handling("getsockopt() error");
    
    state = getsockopt(sock, SOL_SOCKET, SO_RCVBUF, (void*)&rcv_buf, &len); // 입력 버퍼 크기 참조
    if(state)
        error_handling("getsockopt() error");
    
    printf("Input buffer size: %d\n", rcv_buf);
    printf("Output buffer size: %d\n", snd_buf);
    return 0;
}

void error_handling(const char* err_msg)
{
	fputs(err_msg, stderr);
	fputc('\n', stderr);
	exit(1);
}