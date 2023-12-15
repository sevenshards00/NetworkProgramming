/*
* TCP/IP Socket Programming - 네트워크 프로그래밍과 소켓의 이해
* 파일명: fd_seri.c
* 파일 버전: 0.1
* 작성자: Sevenshards
* 작성 일자: 2023-12-15
* 이전 버전 작성 일자:
* 버전 내용: 간단한 파일 디스크립터 예제(3) - 파일과 소켓 모두 생성
* 이전 버전 내용:
*/

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>

int main(int argc, char* argv[])
{
	int fd1, fd2, fd3;
	fd1 = socket(PF_INET, SOCK_STREAM, 0); // 소켓 생성
	fd2 = open("test.dat", O_CREAT|O_WRONLY|O_TRUNC); // 파일 생성
	fd3 = socket(PF_INET,SOCK_DGRAM, 0); // 소켓 생성
	
	printf("file descriptor 1: %d\n", fd1);
	printf("file descriptor 2: %d\n", fd2);
	printf("file descriptor 3: %d\n", fd3);
	
	close(fd1);
	close(fd2);
	close(fd3);
	return 0;
}
