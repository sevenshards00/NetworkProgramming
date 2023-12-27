/*
* TCP/IP Socket Programming - Windows에서의 쓰레드 사용
* 파일명: thread2_win.cpp
* 파일 버전: 0.1
* 작성자: Sevenshards
* 작성 일자: 2023-12-27
* 이전 버전 작성 일자:
* 버전 내용: Windows 기반 쓰레드 생성 예제 - (2)
* 이전 버전 내용:
*/

#include <cstdio>
#include <Windows.h>
#include <process.h>
// unsigned == unsigned int
unsigned WINAPI ThreadFunc(void* arg); // 쓰레드의 메인 함수

int main(int argc, char* argv[])
{
	HANDLE hThread; // Windows에서 쓰레드는 HANDLE!
	DWORD wr; // WaitForSingleObject의 반환값을 받기 위한 변수
	unsigned threadID;
	int param = 5;

	// _beginthreadex()의 반환형은 uintptr_t -> HANDLE로의 강제 형 변환이 필요
	hThread = (HANDLE)_beginthreadex(
		NULL, // 보안 속성 (상속 여부)
		0, // 스택 크기, 0으로 줄 경우 디폴트(1M)
		ThreadFunc, // 쓰레드의 메인이 될 함수 지정
		(LPVOID)&param, // 쓰레드의 함수에 인자로 전달할 값
		0, // 디폴트 생성 flag 지정, 0으로 할 경우 생성과 동시에 실행
		&threadID // 쓰레드 ID를 전달받기 위한 변수의 주소값
	);
	if (hThread == NULL)
	{
		puts("_beginthreadex() error");
		return -1;
	}

	// 쓰레드가 종료될 때까지 (Signaled 상태가 될 때까지) 확인
	// 프로세스와 쓰레드는 종료되면 Signaled, 실행 중이라면 Non-Signaled 상태다.
	// WaitForSingleObject(단일 커널 오브젝트 대상), WaitForMutipleObjects(다수의 커널 오브젝트 대상) 함수를 통해서 Signaled 상태를 확인 가능
	// Linux에서는 pthread_join() 함수를 통해서 쓰레드의 종료 여부를 확인
	// 다만 다른 점은 pthread_join() 함수는 쓰레드의 종료와 함께 소멸까지 처리
	// Windows에서는 CloseHandle을 통해 쓰레드의 Usage Count를 감소시켜줘야 한다.
	if ((wr = WaitForSingleObject(hThread, INFINITE)) == WAIT_FAILED) // 반환값을 제대로 받지 못했을 경우
	{
		puts("thread wait error");
		return -1;
	}
	// 3항 연산자를 이용하여 값을 반환
	// 쓰레드가 Signaled 상태가 되었다면 반환값이 WAIT_OBJECT_0을 반환
	// 그게 아니라면 time-out으로 인한 WAIT_TIMEOUT이 반환
	// INFINITE로 대기하기 때문에 계속 블로킹되므로 해당 값이 나올 일은 없음
	printf("wait result: %s\n", (wr == WAIT_OBJECT_0 ? "siganled" : "time-out"));
	puts("end of main");
	return 0;
}

// _beginthreadex()에서 요구하는 함수의 양식이 다음과 같음
// unsigned (__stdcall* _beginthreadex_proc_type)(void*);
// WINAPI == CALLBACK == __stdcall
// 함수 호출 규약(Calling Convention)
// stdcall의 경우 호출자가 아닌 피호출자(함수)쪽에 스택 프레임 정리하는 코드가 생성
unsigned WINAPI ThreadFunc(void* arg)
{
	int i;
	int cnt = *((int*)arg);
	for (i = 0; i < cnt; i++)
	{
		Sleep(1000);
		puts("running thread");
	}

	return 0;
}