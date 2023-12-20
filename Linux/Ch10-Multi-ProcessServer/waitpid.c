/*
* TCP/IP Socket Programming - 멀티프로세스 기반의 서버 구현
* 파일명: waitpid.c
* 파일 버전: 0.1
* 작성자: Sevenshards
* 작성 일자: 2023-12-20
* 이전 버전 작성 일자:
* 버전 내용: Linux 기반 waitpid 함수 사용 예제
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
    {
        sleep(15);
        return 24;
    }
    else
    {
        while(!waitpid(-1, &status, WNOHANG))
        {
            sleep(3);
            puts("sleep 3 sec");
        }

        if(WIFEXITED(status))
            printf("Child send %d\n", WEXITSTATUS(status));
    }
    return 0;
}