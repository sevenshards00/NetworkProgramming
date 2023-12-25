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
    // 여기서부터 타임아웃을 설정하면 안됨
    // 함수 호출 후에는 구조체 timeval의 멤버 tv_sec과 tv_usec에 저장된 값이 타임아웃이 발생하기까지 남았던 시간으로 바뀌기 때문
    /*
    timeout.tv_sec = 5; // seconds
    timeout.tv_usec = 5000; // microseconds
    */

    while(1)
    {
        // fd_set형 변수 reads의 내용을 복사
        // select 함수 호출이 끝나면 변화가 생긴 파일 디스크립터의 위치를 제외한 나머지 위치의 비트들이 0으로 초기화
        // 원본의 유지를 위해서 복사의 과정을 거쳐야 한다.
        temps = reads;
        // while 내에서 타임아웃을 계속 갱신해야 함
        timeout.tv_sec = 5;
        timeout.tv_usec = 0;
        // select 함수를 호출하는 부분
        // 콘솔로부터 입력된 데이터가 있다면 0보다 큰 수가 반환될 것
        // 아니라면 0, 타임아웃이 일어난 것이다
        // -1일 경우에는 함수 호출이 실패한 것.
        result = select(
            1, // 검사 대상이 되는 파일 디스크립터의 수
            &temps, // '수신된 데이터의 존재여부'에 관심이 있는 파일 디스크립터 정보를 모두 등록한 fd_set 변수의 주소값 전달
            0, // '블로킹 없는 데이터 전송의 가능여부'에 관심이 있는 파일 디스크립터 정보를 모두 등록한 fd_set 변수의 주소값 전달 
            0, // '예외상황의 발생여부'에 관심이 있는 파일 디스크립터 정보를 모두 등록한 fd_set 변수의 주소값 전달
            &timeout // select 함수 호출 이후 무한정 블로킹 상태에 빠지지 않도록 타임아웃을 설정
            );
        if(result == -1) // select 함수 호출을 실패한 경우
        {
            puts("select() error");
            break;
        }
        else if(result == 0) // 타임아웃이 발생한 경우
            puts("Time-out!");
        else
        {
            if(FD_ISSET(0, &temps)) // 변화가 발생한 파일 디스크립터가 표준 입력(0)인지를 확인
            {
                // 표준 입력(콘솔)에서 받아들인 데이터를 다시 화면에 출력
                str_len = read(0, buf, BUF_SIZE);
                buf[str_len] = 0;
                printf("Message from console: %s", buf);
            }
        }
    }

    return 0;
}