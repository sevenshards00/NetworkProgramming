/*
* TCP/IP Socket Programming - 소켓의 다양한 옵션
* 파일명: sock_type.c
* 파일 버전: 0.1
* 작성자: Sevenshards
* 작성 일자: 2023-12-20
* 이전 버전 작성 일자:
* 버전 내용: Linux 기반 getsockopt 함수 사용 예제
* 이전 버전 내용:
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>

void error_handling(const char* err_msg);

int main(int argc, char* argv[])
{
    int tcp_sock, udp_sock;
    int sock_type;
    socklen_t optlen;
    int state;

    optlen = sizeof(sock_type);
    tcp_sock = socket(PF_INET, SOCK_STREAM, 0);
    udp_sock = socket(PF_INET, SOCK_DGRAM, 0);
    printf("SOCK_STREAM:%d\n", SOCK_STREAM);
    printf("SOCK_DGRAM:%d\n", SOCK_DGRAM);

    state = getsockopt(
        tcp_sock, // 옵션 확인을 위한 소켓의 파일 디스크립터
        SOL_SOCKET, // 확인할 옵션의 프로토콜 레벨
        SO_TYPE, // 확인할 옵션의 이름
        (void*)&sock_type, // 확인 결과의 저장을 위한 버퍼의 주소값을 전달
        &optlen // 확인 결과를 저장하기 위한 버퍼의 크기를 담고 있는 변수의 주소값, 크기는 바이트 단위
        );
    if(state)
        error_handling("getsockopt() error");
    printf("Socket type one: %d\n", sock_type);

    state = getsockopt(
        udp_sock,
        SOL_SOCKET,
        SO_TYPE,
        (void*)&sock_type,
        &optlen
        );
    if(state)
        error_handling("getsockopt() error");
    printf("Socket type two: %d\n", sock_type);
    return 0;
}

void error_handling(const char* err_msg)
{
	fputs(err_msg, stderr);
	fputc('\n', stderr);
	exit(1);
}