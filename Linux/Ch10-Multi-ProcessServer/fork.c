/*
* TCP/IP Socket Programming - 멀티프로세스 기반의 서버 구현
* 파일명: fork.c
* 파일 버전: 0.1
* 작성자: Sevenshards
* 작성 일자: 2023-12-20
* 이전 버전 작성 일자:
* 버전 내용: Linux 기반 자식 프로세스 생성 예제 - fork()
* 이전 버전 내용:
*/

#include <stdio.h>
#include <unistd.h>

int gval = 10; // 전역 변수

int main(int argc, char* argv[])
{
    pid_t pid;
    int lval = 20; // 지역 변수
    gval++, lval += 5;

    pid = fork();
    if(pid == 0) // 만약 자식 프로세스라면 생성시 pid == 0
        gval += 2, lval += 2;
    else // 아니라면
        gval -= 2, lval -= 2;
    
    if(pid == 0)
        printf("Child proc: [gval == %d, lval == %d]\n", gval, lval);
    else
        printf("Parent proc: [gval == %d, lval == %d]\n", gval, lval);
    return 0;
}