/*
* TCP/IP Socket Programming - Windows에서의 쓰레드 사용
* 파일명: thread1_win.c
* 파일 버전: 0.1
* 작성자: Sevenshards
* 작성 일자: 2023-12-27
* 이전 버전 작성 일자:
* 버전 내용: Windows 기반 쓰레드 생성 예제 - (1)
* 이전 버전 내용:
*/

#include <stdio.h>
#include <Windows.h>
#include <process.h>
// unsigned == unsigned int
unsigned WINAPI ThreadFunc(void* arg); // 쓰레드의 메인 함수

int main(int argc, char* argv[])
{
	HANDLE hThread; // Windows에서 쓰레드는 HANDLE!
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

	Sleep(3000);
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