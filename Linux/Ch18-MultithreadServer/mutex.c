/*
* TCP/IP Socket Programming - 멀티쓰레드 기반의 서버 구현
* 파일명: mutex.c
* 파일 버전: 0.1
* 작성자: Sevenshards
* 작성 일자: 2023-12-27
* 이전 버전 작성 일자:
* 버전 내용: mutex를 이용한 쓰레드 동기화
* 이전 버전 내용:
*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_THREAD 100

void* thread_inc(void* arg);
void* thread_des(void* arg);
long long num = 0;
pthread_mutex_t mutex;

int main(int argc, char* argv[])
{
    pthread_t thread_id[NUM_THREAD];
    int i;

    pthread_mutex_init(&mutex, NULL);

    printf("sizeof long long: %d\n", sizeof(long long));
    for(i = 0; i < NUM_THREAD; i++)
    {
        if(i % 2)
            pthread_create(&(thread_id[i]), NULL, thread_inc, NULL);
        else
            pthread_create(&(thread_id[i]), NULL, thread_des, NULL);
    }

    for(i = 0; i < NUM_THREAD; i++)
        pthread_join(thread_id[i], NULL);
    
    printf("result: %lld\n", num);
    pthread_mutex_destroy(&mutex);
    return 0;
}

void* thread_inc(void* arg)
{
    int i;
    for(i = 0; i < 50000000; i++)
    {
        pthread_mutex_lock(&mutex);
        num += 1;
        pthread_mutex_unlock(&mutex);
    }
        
    return NULL;
}

void* thread_des(void* arg)
{
    int i;
    for(i = 0; i < 50000000; i++)
    {
        pthread_mutex_lock(&mutex);
        num -= 1;
        pthread_mutex_unlock(&mutex);
    }
        
    return NULL;
}