/*
* TCP/IP Socket Programming - 멀티쓰레드 기반의 서버 구현
* 파일명: thread1.c
* 파일 버전: 0.1
* 작성자: Sevenshards
* 작성 일자: 2023-12-27
* 이전 버전 작성 일자:
* 버전 내용: 간단한 쓰레드 생성 및 실행 예제
* 이전 버전 내용:
*/

#include <stdio.h>
#include <pthread.h>

void* thread_main(void* arg);

int main(int argc, char* argv[])
{
    pthread_t t_id;
    int thread_param = 5;

    if(pthread_create(&t_id, NULL, thread_main, (void*)&thread_param) != 0)
    {
        puts("pthread_create() error");
        return -1;
    }
    sleep(10);
    puts("end of main");
    return 0;
}

void* thread_main(void* arg)
{
    int i;
    int cnt = *((int*)arg);
    for(i = 0; i < cnt; i++)
    {
        sleep(1);
        puts("running thread");
    }
    return NULL;
}