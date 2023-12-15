/*
* TCP/IP Socket Programming - 네트워크 프로그래밍과 소켓의 이해
* 파일명: low_open.c
* 파일 버전: 0.1
* 작성자: Sevenshards
* 작성 일자: 2023-12-15
* 이전 버전 작성 일자:
* 버전 내용: 간단한 파일 디스크립터 예제
* 이전 버전 내용:
*/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
void error_handling(char* message);

int main(int argc, char* argv[])
{
	int fd;
	char buf[] = "Let's go!\n";
	
	fd = open("data.txt", O_CREAT|O_WRONLY|O_TRUNC);
	if(fd == -1)
		error_handling("open() error!");
	printf("file descriptor: %d\n",fd);
	
	if(write(fd, buf, sizeof(buf)) == -1)
		error_handling("write() error!");
	close(fd);
	return 0;
}

void error_handling(char* message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
