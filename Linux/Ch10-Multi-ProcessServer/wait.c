/*
* TCP/IP Socket Programming - 멀티프로세스 기반의 서버 구현
* 파일명: wait.c
* 파일 버전: 0.1
* 작성자: Sevenshards
* 작성 일자: 2023-12-20
* 이전 버전 작성 일자:
* 버전 내용: Linux 기반 wait 함수 사용 예제
* 이전 버전 내용:
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char* argv[])
{
    int status;
    pid_t pid = fork();

    if(pid == 0)
        return 3;
    else
    {
        printf("Child PID: %d\n", pid);
        pid = fork();
        if(pid == 0)
            exit(7);
        else
        {
            printf("Child PID: %d\n", pid);
            wait(&status);
            if(WIFEXITED(status))
                printf("First Child send: %d\n", WEXITSTATUS(status));

            wait(&status);
            if(WIFEXITED(status))
                printf("Second Child send: %d\n", WEXITSTATUS(status));
            sleep(30);
        }
    }
    return 0;
}