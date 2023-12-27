/*
* TCP/IP Socket Programming - 소켓과 표준 입출력
* 파일명: stdcpy.c
* 파일 버전: 0.1
* 작성자: Sevenshards
* 작성 일자: 2023-12-27
* 이전 버전 작성 일자:
* 버전 내용: 표준 입출력 함수를 이용한 파일 복사
* 이전 버전 내용:
*/

#include <stdio.h>
#define BUF_SIZE 3

int main(int argc, char* argv[])
{
    FILE* fp1;
    FILE* fp2;
    char buf[BUF_SIZE];

    fp1 = fopen("Shakespeare.txt", "r");
    fp2 = fopen("ShakespeareCpy.txt", "w");

    while(fgets(buf, BUF_SIZE, fp1) != NULL)
        fputs(buf, fp2);

    fclose(fp1);
    fclose(fp2);
    return 0;
}