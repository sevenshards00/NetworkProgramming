/*
* TCP/IP Socket Programming - 멀티쓰레드 기반의 서버 구현
* 파일명: thread1.c
* 파일 버전: 0.1
* 작성자: Sevenshards
* 작성 일자: 2023-12-27
* 이전 버전 작성 일자:
* 버전 내용: pthread_join 함수 사용 예제
* 이전 버전 내용:
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

void* thread_main(void* arg);

int main(int argc, char* argv[])
{
    pthread_t t_id;
    int thread_param = 5;
    void* thr_ret;

    if(pthread_create(&t_id, NULL, thread_main, (void*)&thread_param) != 0)
    {
        puts("pthread_create() error");
        return -1;
    }

    if(pthread_join(t_id, &thr_ret) != 0)
    {
        puts("pthread_join() error");
        return -1;
    }

    printf("Thread return message: %s\n", (char*)thr_ret);
    free(thr_ret);
    return 0;
}

void* thread_main(void* arg)
{
    int i;
    int cnt = *((int*)arg);
    char* msg = (char*)malloc(sizeof(char)*50);
    strcpy(msg, "Hello, I'm thread!\n");

    for(i = 0; i < cnt; i++)
    {
        sleep(1);
        puts("running thread");
    }
    return (void*)msg;
}