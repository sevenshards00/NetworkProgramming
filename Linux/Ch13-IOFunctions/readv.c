/*
* TCP/IP Socket Programming - 다양한 입출력 함수들
* 파일명: readv.c
* 파일 버전: 0.1
* 작성자: Sevenshards
* 작성 일자: 2023-12-26
* 이전 버전 작성 일자:
* 버전 내용: readv 사용 예제
* 이전 버전 내용:
*/

#include <stdio.h>
#include <sys/uio.h>

#define BUF_SIZE 100

int main(int argc, char* argv[])
{
    struct iovec vec[2];
    char buf1[BUF_SIZE] = { 0, };
    char buf2[BUF_SIZE] = { 0, };
    int str_len;

    vec[0].iov_base = buf1;
    vec[0].iov_len = 5;
    vec[1].iov_base = buf2;
    vec[1].iov_len = BUF_SIZE;

    // 0은 표준 입력 (stdin) 파일 디스크립터를 지정
    str_len = readv(0, vec, 2);
    printf("Read Bytes: %d\n", str_len);
    printf("First Message: %s\n", buf1);
    printf("Second Message: %s\n", buf2);
    return 0;
}