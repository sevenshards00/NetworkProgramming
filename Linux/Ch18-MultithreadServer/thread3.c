/*
* TCP/IP Socket Programming - 멀티쓰레드 기반의 서버 구현
* 파일명: thread3.c
* 파일 버전: 0.1
* 작성자: Sevenshards
* 작성 일자: 2023-12-27
* 이전 버전 작성 일자:
* 버전 내용: 두 개 이상의 쓰레드를 생성하는 예제 - (1)
* 이전 버전 내용:
*/

#include <stdio.h>
#include <pthread.h>

void* thread_summation(void* arg);
int sum = 0;

int main(int argc, char* argv[])
{
    pthread_t id_t1, id_t2;
    int range1[] = {1, 5};
    int range2[] = {6, 10};

    pthread_create(&id_t1, NULL, thread_summation, (void*)range1);
    pthread_create(&id_t2, NULL, thread_summation, (void*)range2);
    
    pthread_join(id_t1, NULL);
    pthread_join(id_t2, NULL);

    printf("Result: %d\n", sum);
    return 0;
}

void* thread_summation(void* arg)
{
    int start = ((int*)arg)[0];
    int end = ((int*)arg)[1];

    while(start <= end)
    {
        sum += start;
        start++;
    }
    
    return NULL;
}