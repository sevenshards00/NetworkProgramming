/*
* TCP/IP Socket Programming - Windows에서의 쓰레드 동기화
* 파일명: SyncEvent_win.cpp
* 파일 버전: 0.1
* 작성자: Sevenshards
* 작성 일자: 2023-12-28
* 이전 버전 작성 일자:
* 버전 내용: 커널 모드 동기화(3) - Event 기반의 동기화 (메모리 접근, 실행 순서 동기화)
* 이전 버전 내용:
*/

#include <iostream>
#include <cstdio>
#include <Windows.h>
#include <process.h>
using std::cin;
using std::cout;

#define STR_LEN 100

// unsigned == unsigned int
// Linux 버전의 semaphore.c와 동일한 구조를 Windows 기반으로 바꾼 코드
unsigned WINAPI NumberOfA(void* arg); // 증가 함수
unsigned WINAPI NumberOfOthers(void* arg); // 감소 함수

static char str[STR_LEN];
static HANDLE hEvent;

// 생산자-소비자 모델
// main 쓰레드는 생산자
// 두 개의 쓰레드는 소비자
// main 쓰레드가 작업을 마치면 Event를 Signaled 상태로
// 소비자는 Signaled 상태가 된 것을 확인하면 그때부터 실행
int main(int argc, char* argv[])
{
	HANDLE hThread1, hThread2;
	hEvent = CreateEvent(
		NULL, // 보안(상속) 속성, 상속하지 않으므로 NUL
		TRUE, // manual-reset mode로, FALSE로 주면 auto-reset mode
		FALSE, // Non-Signaled 상태로 생성
		NULL // 이름 설정, 여기서는 안쓰므로 NULL
	);

	hThread1 = (HANDLE)_beginthreadex(NULL, 0, NumberOfA, NULL, 0, NULL);
	hThread2 = (HANDLE)_beginthreadex(NULL, 0, NumberOfOthers, NULL, 0, NULL);

	cout << "Input String: ";
	cin >> str;
	SetEvent(hEvent); // Signaled 상태로 변경
	WaitForSingleObject(hThread1, INFINITE);
	WaitForSingleObject(hThread2, INFINITE);
	ResetEvent(hEvent); // Non-Signaled 상태로 다시 변경, 예제 코드에서는 더 수행할 작업이 없으므로 할 필요는 없음

	CloseHandle(hEvent);
	CloseHandle(hThread1);
	CloseHandle(hThread2);
	return 0;
}

unsigned WINAPI NumberOfA(void* arg)
{
	int i, cnt = 0;
	WaitForSingleObject(hEvent, INFINITE);
	for (i = 0; str[i] != '\0'; i++)
	{
		if (str[i] == 'A')
			cnt++;
	}
	// cout 쓸 경우 동시 접근 문제를 확인 가능
	// 표준 입출력 함수보다 좀 느린것으로 판단됨
	// 뮤텍스나 세마포어를 이용하여 추가적으로 동기화 필요
	// 	cout << "Num of A: " << cnt << '\n';
	printf("Num of A: %d\n", cnt);
	return 0;
}

unsigned WINAPI NumberOfOthers(void* arg)
{
	int i, cnt = 0;
	WaitForSingleObject(hEvent, INFINITE);
	for (i = 0; str[i] != '\0'; i++)
	{
		if (str[i] != 'A')
			cnt++;
	}
	// cout 쓸 경우 동시 접근 문제를 확인 가능
	// 표준 입출력 함수보다 좀 느린것으로 판단됨
	// 뮤텍스나 세마포어를 이용하여 추가적으로 동기화 필요
	// cout << "Num of others: " << cnt << '\n';
	printf("Num of others: %d\n", cnt);
	return 0;
}