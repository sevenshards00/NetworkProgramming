/*
* TCP/IP Socket Programming - 소켓의 다양한 옵션
* 파일명: set_buf.c
* 파일 버전: 0.1
* 작성자: Sevenshards
* 작성 일자: 2023-12-20
* 이전 버전 작성 일자:
* 버전 내용: Linux 기반 setsockopt 함수 사용 예제 - 입출력 버퍼 크기 설정
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
    int snd_buf = 1024 * 3, rcv_buf = 1024 * 3; // 입/출력 버퍼를 3바이트로 지정
    int state;
    socklen_t len;

    sock = socket(PF_INET, SOCK_STREAM, 0);
    
    // getsockopt와 인자가 동일하다
    // 단, 마지막 인자는 인자가 포인터가 아니므로 주소값이 아닌 크기를 전달하면 된다.
    state = setsockopt(sock, SOL_SOCKET, SO_SNDBUF, (void*)&snd_buf, sizeof(snd_buf));
    if(state)
        error_handling("setsockopt() error");

    state = setsockopt(sock, SOL_SOCKET, SO_SNDBUF, (void*)&rcv_buf, sizeof(rcv_buf));
    if(state)
        error_handling("setsockopt() error");

    len = sizeof(snd_buf);
    state = getsockopt(sock, SOL_SOCKET, SO_SNDBUF, (void*)&snd_buf, &len);
    if(state)
        error_handling("getsockopt() error");
    
    state = getsockopt(sock, SOL_SOCKET, SO_RCVBUF, (void*)&rcv_buf, &len);
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