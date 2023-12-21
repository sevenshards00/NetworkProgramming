/*
* TCP/IP Socket Programming - 프로세스간 통신(Inter Process Communication)
* 파일명: pipe3.c
* 파일 버전: 0.1
* 작성자: Sevenshards
* 작성 일자: 2023-12-21
* 이전 버전 작성 일자:
* 버전 내용: 파이프(Pipe) 기반 프로세스간 통신 예제 - 양방향 통신 개선
* 이전 버전 내용:
*/

#include <stdio.h>
#include <unistd.h>
#define BUF_SIZE 30

int main(int argc, char* argv[])
{
    int fds1[2], fds2[2]; // pipe에 사용할 파일 디스크립터
    char str1[] = "Who are you?";
    char str2[] = "I'm your father";
    char buf[BUF_SIZE];
    pid_t pid;

    // 파이프 생성
    // fds[0]은 데이터를 받을 때(수신) 사용 -> 파이프의 출구
    // fds[1]은 데이터를 보낼 때(송신) 사용 -> 파이프의 입구
    // 파이프 하나로만 양방향 통신하는 것에는 문제가 있다.
    // 이전 pipe2.c에서 자식 프로세스의 sleep(2)를 주석처리하게 될 경우에는 문제가 생긴다.
    // 자기가 파이프로 데이터를 보내놓고 자기가 다시 가져가버린다.
    // 그래서 부모 프로세스쪽에서는 죽어도 데이터를 못받는 무한 대기 상태에 빠진다.
    // 확실한 양방향 통신을 위해 파이프를 2개를 사용한다.
    // Windows 기준으로는 anonymous pipe를 2개 사용하는 것과 동일하다.
    pipe(fds1), pipe(fds2);
    pid = fork(); // 자식 프로세스 생성
    if(pid == 0)
    {
        write(fds1[1], str1, sizeof(str1)); // 자식 프로세스일 경우에 메시지를 전송
        read(fds2[0], buf, BUF_SIZE); // 부모 프로세스로부터 메시지를 수신
        printf("Child proc output: %s\n", buf);
    }
        
    else
    {
        read(fds1[0], buf, BUF_SIZE); // 부모 프로세스에서는 자식 프로세스가 보낸 메시지를 받는다.
        printf("Parent proc output: %s\n", buf);
        write(fds2[1], str2, sizeof(str2)); // 자식 프로세스에게 메시지를 전송
        sleep(3);
    }
    return 0;
}