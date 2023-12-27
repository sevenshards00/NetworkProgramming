/*
* TCP/IP Socket Programming - 소켓과 표준 입출력
* 파일명: todes.c
* 파일 버전: 0.1
* 작성자: Sevenshards
* 작성 일자: 2023-12-27
* 이전 버전 작성 일자:
* 버전 내용: fileno 함수를 이용한 File descriptor 변환 예제
* 이전 버전 내용:
*/

#include <stdio.h>
#include <fcntl.h>

int main(int argc, char* argv[])
{
    FILE* fp = NULL;
    int fd = open("data.dat", O_WRONLY|O_CREAT|O_TRUNC);
    if(fd == -1)
    {
        fputs("file open error", stdout);
        return -1;
    }

    printf("First file descriptor: %d\n", fd);
    fp = fdopen(fd, "w"); // File Descriptor to FILE*
    fputs("TCP/IP Socket Programming\n", fp);
    printf("Second file descriptor: %d\n", fileno(fp)); // FILE* to File Descriptor
    fclose(fp);
    return 0;
}