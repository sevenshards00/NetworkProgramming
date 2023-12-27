/*
* TCP/IP Socket Programming - 입출력 스트림의 분리에 대한 나머지 이야기
* 파일명: dup.c
* 파일 버전: 0.1
* 작성자: Sevenshards
* 작성 일자: 2023-12-27
* 이전 버전 작성 일자:
* 버전 내용: 파일 디스크립터 복사 예제 - dup, dup2
* 이전 버전 내용:
*/

#include <stdio.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
    int cfd1, cfd2;
    char str1[] = "Hello!\n";
    char str2[] = "It's a good day\n";

    cfd1 = dup(1); // 표준 출력(1) 파일 디스크립터를 복사
    cfd2 = dup2(cfd1, 7); // 파일 디스크립터 cfd1를 복사, 파일 디스크립터의 값을 7로 지정

    printf("fd1 = %d, fd2 = %d\n", cfd1, cfd2);
    write(cfd1, str1, sizeof(str1));
    write(cfd2, str2, sizeof(str2));

    // 복사한 파일 디스크립터를 소멸
    close(cfd1);
    close(cfd2);
    
    write(1, str1, sizeof(str1)); // 표준 출력을 대상으로 write
    close(1); // 표준 출력 파일 디스크립터 소멸
    write(1, str2, sizeof(str2)); // 출력되지 않음

    return 0;
}