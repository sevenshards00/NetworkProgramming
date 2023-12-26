/*
* TCP/IP Socket Programming - 다양한 입출력 함수들
* 파일명: writev.c
* 파일 버전: 0.1
* 작성자: Sevenshards
* 작성 일자: 2023-12-26
* 이전 버전 작성 일자:
* 버전 내용: writev 사용 예제
* 이전 버전 내용:
*/

#include <stdio.h>
#include <sys/uio.h>

int main(int argc, char* argv[])
{
    struct iovec vec[2];
    char buf1[] = "ABCDEFG";
    char buf2[] = "1234567";
    int str_len;

    vec[0].iov_base = buf1;
    vec[0].iov_len = 3;
    vec[1].iov_base = buf2;
    vec[1].iov_len = 3;

    // writev의 첫 번째 인자는 파일 디스크립터
    // 1은 표준 출력(stdout)
    // vec 구조체에 있는 총 2개의 배열을 write한다
    str_len = writev(1, vec, 2);
    puts("");
    printf("Write bytes: %d\n", str_len);
    return 0;
}