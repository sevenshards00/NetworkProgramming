/*
* TCP/IP Socket Programming - 소켓과 표준 입출력
* 파일명: syscpy.c
* 파일 버전: 0.1
* 작성자: Sevenshards
* 작성 일자: 2023-12-27
* 이전 버전 작성 일자:
* 버전 내용: 시스템 함수를 이용한 파일 복사
* 이전 버전 내용:
*/

#include <stdio.h>
#include <fcntl.h>

#define BUF_SIZE 3

int main(int argc, char* argv[])
{
    int fd1, fd2; // FILE이 아닌 file descriptor 사용
    int len;
    char buf[BUF_SIZE];

    fd1 = open("Shakespeare.txt", O_RDONLY);
    fd2 = open("ShakespeareCpy.txt", O_WRONLY|O_CREAT|O_TRUNC);

    while((len = read(fd1, buf, sizeof(buf)) > 0))
        write(fd2, buf, len);

    close(fd1);
    close(fd2);
    return 0;
}