/*
* TCP/IP Socket Programming - 멀티프로세스 기반의 서버 구현
* 파일명: remove_zombie.c
* 파일 버전: 0.1
* 작성자: Sevenshards
* 작성 일자: 2023-12-20
* 이전 버전 작성 일자:
* 버전 내용: Linux 기반 시그널 핸들링을 이용한 좀비 프로세스 소멸 예제
* 이전 버전 내용:
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

void read_childproc(int sig)
{
    int status;
    pid_t id = waitpid(-1, &status, WNOHANG);
    if(WIFEXITED(status))
    {
        printf("Removed proc id: %d\n", id);
        printf("Child send: %d\n", WEXITSTATUS(status));
    }
}

int main(int argc, char* argv[])
{
    pid_t pid;
    struct sigaction act; // sigaction 구조체 선언
    act.sa_handler = read_childproc; // sigaction의 핸들러 함수를 지정
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    sigaction(SIGCHLD, &act, 0);

    pid = fork();
    if(pid == 0) // 자식 프로세스가 실행되는 영역
    {
        puts("Hi, I'm child process");
        sleep(10);
        return 12; // return을 이용한 종료
    }
    else // 부모 프로세스가 실행되는 영역
    {
        printf("Child proc id: %d\n", pid);
        pid = fork(); // 또 다른 자식 프로세스 생성
        if(pid == 0)
        {
            puts("Hi! I'm child process too!");
            sleep(10);
            exit(24); // exit를 이용한 종료
        }
        else
        {
            int i;
            printf("Child proc id: %d\n", pid);
            for(i = 0; i < 5; i++)
            {
                puts("wait...");
                sleep(5);
            }
        }
    }
    return 0;
}