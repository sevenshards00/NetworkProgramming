/*
* TCP/IP Socket Programming - 네트워크 프로그래밍과 소켓의 이해
* 파일명: low_read.c
* 파일 버전: 0.1
* 작성자: Sevenshards
* 작성 일자: 2023-12-15
* 이전 버전 작성 일자:
* 버전 내용: 간단한 파일 디스크립터 예제(2) - read
* 이전 버전 내용:
*/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#define BUF_SIZE 100

void error_handling(char* message);

int main(int argc, char* argv[])
{
	int fd;
	char buf[BUF_SIZE];
	
	fd = open("data.txt", O_RDONLY);
	if(fd == -1)
		error_handling("open() error!");
	printf("file descriptor: %d\n",fd);
	
	if(read(fd, buf, sizeof(buf)) == -1)
		error_handling("read() error!");
	printf("file data: %s", buf);
	close(fd);
	return 0;
}

void error_handling(char* message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
