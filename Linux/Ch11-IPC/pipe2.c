/*
* TCP/IP Socket Programming - 프로세스간 통신(Inter Process Communication)
* 파일명: pipe2.c
* 파일 버전: 0.1
* 작성자: Sevenshards
* 작성 일자: 2023-12-21
* 이전 버전 작성 일자:
* 버전 내용: 파이프(Pipe) 기반 프로세스간 통신 예제 - 양방향 통신
* 이전 버전 내용:
*/

#include <stdio.h>
#include <unistd.h>
#define BUF_SIZE 30

int main(int argc, char* argv[])
{
    int fds[2]; // pipe에 사용할 파일 디스크립터
    char str1[] = "Who are you?";
    char str2[] = "I'm your father";
    char buf[BUF_SIZE];
    pid_t pid;

    // 파이프 생성
    // fds[0]은 데이터를 받을 때(수신) 사용 -> 파이프의 출구
    // fds[1]은 데이터를 보낼 때(송신) 사용 -> 파이프의 입구
    pipe(fds);
    pid = fork(); // 자식 프로세스 생성
    if(pid == 0)
    {
        write(fds[1], str1, sizeof(str1)); // 자식 프로세스일 경우에 메시지를 전송
        sleep(2); // 2초간 프로세스 중단.
        read(fds[0], buf, BUF_SIZE); // 부모 프로세스로부터 메시지를 수신
        printf("Child proc output: %s\n", buf);
    }
        
    else
    {
        read(fds[0], buf, BUF_SIZE); // 부모 프로세스에서는 자식 프로세스가 보낸 메시지를 받는다.
        printf("Parent proc output: %s\n", buf);
        write(fds[1], str2, sizeof(str2)); // 자식 프로세스에게 메시지를 전송
        sleep(3); // 3초간 프로세스 중단
    }
    return 0;
}