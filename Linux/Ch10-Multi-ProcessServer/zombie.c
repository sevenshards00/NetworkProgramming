/*
* TCP/IP Socket Programming - 멀티프로세스 기반의 서버 구현
* 파일명: zombie.c
* 파일 버전: 0.1
* 작성자: Sevenshards
* 작성 일자: 2023-12-20
* 이전 버전 작성 일자:
* 버전 내용: Linux 기반 좀비 프로세스 생성 예제
* 이전 버전 내용:
*/

#include <stdio.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
    pid_t pid = fork();

    if(pid == 0) // 자식 프로세스라면
        puts("Hi, I'm a child process");
    else
    {
        printf("Child Process ID: %d\n", pid);
        sleep(30); // 30초 동안 프로세스를 멈춘다.
    }

    if(pid == 0)
        puts("End child process");
    else
        puts("End parent process");
    return 0;
}