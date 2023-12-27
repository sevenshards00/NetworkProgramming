/*
* TCP/IP Socket Programming - 소켓과 표준 입출력
* 파일명: desto.c
* 파일 버전: 0.1
* 작성자: Sevenshards
* 작성 일자: 2023-12-27
* 이전 버전 작성 일자:
* 버전 내용: fdopen 함수를 이용한 FILE 구조체 포인터 변환 예제
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

    fp = fdopen(fd, "w"); // File Descriptor to FILE*
    fputs("Network C Programming\n", fp);
    fclose(fp);
    return 0;
}