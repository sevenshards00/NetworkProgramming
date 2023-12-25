/*
* TCP/IP Socket Programming - I/O 멀티플렉싱(Multiplexing)
* 파일명: select.c
* 파일 버전: 0.1
* 작성자: Sevenshards
* 작성 일자: 2023-12-25
* 이전 버전 작성 일자:
* 버전 내용: select 함수 사용 예제
* 이전 버전 내용:
*/

#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/select.h>
#define BUF_SIZE 30

int main(int argc, char* argv[])
{
    fd_set reads, temps;
    int result, str_len;
    char buf[BUF_SIZE];
    struct timeval timeout;

    FD_ZERO(&reads); // fd_set형 변수 reads의 모든 비트를 0으로 초기화
    FD_SET(0, &reads); // reads에 파일 디스크립터 정보 등록, 0은 표준 입력(콘솔)

    // select 함수 호출 이후 무한정 블로킹 상태에 빠지지 않도록 타임아웃 설정
    // 5초의 타임아웃을 설정
    /*
    timeout.tv_sec = 5; // seconds
    timeout.tv_usec = 5000; // microseconds
    */

    while(1)
    {
        temps = reads;
        timeout.tv_sec = 5;
        timeout.tv_usec = 0;
        result = select(1, &temps, 0, 0, &timeout);
        if(result == -1)
        {
            puts("select() error");
            break;
        }
        else if(result == 0)
            puts("Time-out!");
        else
        {
            if(FD_ISSET(0, &temps))
            {
                str_len = read(0, buf, BUF_SIZE);
                buf[str_len] = 0;
                printf("Message from console: %s", buf);
            }
        }
    }

    return 0;
}