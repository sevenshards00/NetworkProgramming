/*
* TCP/IP Socket Programming - 멀티프로세스 기반의 서버 구현
* 파일명: waitpid.c
* 파일 버전: 0.1
* 작성자: Sevenshards
* 작성 일자: 2023-12-20
* 이전 버전 작성 일자:
* 버전 내용: Linux 기반 시그널 핸들링 예제 - signal, alarm
* 이전 버전 내용:
*/

#include <stdio.h>
#include <unistd.h>
#include <signal.h>

void timeout(int sig)
{
    if(sig == SIGALRM)
        puts("Time out!");
    alarm(2);
}

void keycontrol(int sig)
{
    if(sig == SIGINT)
        puts("CTRL+C Pressed");
}

int main(int argc, char* argv[])
{
    int i;
    // void (*signal(int signo, void(*func)(int))(int));
    // 함수의 이름이 signal
    // 인자는 int signo, void(*func)(int)를 받는다.
    // 반환형은 '매개변수형이 int이고 반환형이 void인 함수 포인터'를 반환
    signal(SIGALRM, timeout);
    signal(SIGINT, keycontrol);
    alarm(2);

    for(i=0; i<3; i++)
    {
        puts("wait...");
        sleep(100);
    }
    return 0;
}