/*
* TCP/IP Socket Programming - 멀티쓰레드 기반의 서버 구현
* 파일명: semaphore.c
* 파일 버전: 0.1
* 작성자: Sevenshards
* 작성 일자: 2023-12-27
* 이전 버전 작성 일자:
* 버전 내용: semaphore(binary semaphore)를 이용한 쓰레드 동기화(메모리 접근, 실행 순서)
* 이전 버전 내용:
*/

#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

void* read(void* arg);
void* accu(void* arg);
static sem_t sem_one;
static sem_t sem_two;
static int num;

int main(int argc, char* argv[])
{
    pthread_t id_t1, id_t2;
    sem_init(&sem_one, 0, 0);
    sem_init(&sem_two, 0, 1);
    pthread_create(&id_t1, NULL, read, NULL);
    pthread_create(&id_t2, NULL, accu, NULL);
    
    pthread_join(id_t1, NULL);
    pthread_join(id_t2, NULL);
    
    sem_destroy(&sem_one);
    sem_destroy(&sem_two);
    return 0;
}

void* read(void* arg)
{
    int i;
    for(i = 0; i < 5; i++)
    {
        fputs("Input num: ", stdout);

        sem_wait(&sem_two);
        scanf("%d", &num);
        sem_post(&sem_one);
    }
        
    return NULL;
}

void* accu(void* arg)
{
    int sum = 0, i;
    for(i = 0; i < 5; i++)
    {
        sem_wait(&sem_one);
        sum += num;
        sem_post(&sem_two);
    }
    printf("Result: %d\n", sum);
    return NULL;
}