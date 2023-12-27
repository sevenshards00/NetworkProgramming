/*
* TCP/IP Socket Programming - Windows에서의 쓰레드 동기화
* 파일명: SyncInterlocked_win.cpp
* 파일 버전: 0.1
* 작성자: Sevenshards
* 작성 일자: 2023-12-27
* 이전 버전 작성 일자:
* 버전 내용: 유저 모드 동기화(2) - Interlock 기반의 동기화
* 이전 버전 내용:
*/

#include <iostream>
#include <cstdio>
#include <Windows.h>
#include <process.h>
using std::cout;

#define NUM_THREAD 50
// unsigned == unsigned int
// Linux 버전의 thread4.c와 동일한 구조를 Windows 기반으로 바꾼 코드
unsigned WINAPI threadInc(void* arg); // 증가 함수
unsigned WINAPI threadDes(void* arg); // 감소 함수

LONGLONG num = 0;
//CRITICAL_SECTION cs; // CRITICAL_SECTION 오브젝트 선언

int main(int argc, char* argv[])
{
	HANDLE tHandles[NUM_THREAD] = { 0, }; // HANDLE을 배열로 선언

	//InitializeCriticalSection(&cs); // CRITICAL_SECTION 오브젝트 초기화
	for (DWORD i = 0; i < NUM_THREAD; i++)
	{
		// 익숙해질 때까지만 _beginthreadex() 함수 인자를 다음과 같이 쪼개서 사용
		if (i % 2)
			tHandles[i] = (HANDLE)_beginthreadex(
				NULL, // 보안 속성(상속 여부)
				0, // 쓰레드의 스택 크기, 0으로 지정할 경우 디폴트 값(1M)
				threadInc, // 쓰레드의 main이 될 함수
				NULL, // 쓰레드 함수에 전달할 인자, 없다면 NULL
				0, // 플래그, 0으로 둘 경우 생성과 동시에 실행, CREATE_SUSPENDED를 사용하면 Blocked 상태로 시작. Suspend Count 개념 참조
				NULL // 쓰레드의 ID를 저장할 변수의 주소값 인자로 전달, 필요 없다면 NULL
			);
		else
			tHandles[i] = (HANDLE)_beginthreadex(
				NULL, // 보안 속성(상속 여부)
				0, // 쓰레드의 스택 크기, 0으로 지정할 경우 디폴트 값(1M)
				threadDes, // 쓰레드의 main이 될 함수
				NULL, // 쓰레드 함수에 전달할 인자, 없다면 NULL
				0, // 플래그, 0으로 둘 경우 생성과 동시에 실행, CREATE_SUSPENDED를 사용하면 Blocked 상태로 시작. Suspend Count 개념 참조
				NULL // 쓰레드의 ID를 저장할 변수의 주소값 인자로 전달, 필요 없다면 NULL
			);

	}
	// 원래는 쓰레드가 생성이 되었는가 예외 상황을 처리해야 하나 코드가 길어지므로 패스.


	// 쓰레드가 종료될 때까지 (Signaled 상태가 될 때까지) 확인
	// 프로세스와 쓰레드는 종료되면 Signaled, 실행 중이라면 Non-Signaled 상태다.
	// WaitForSingleObject(단일 커널 오브젝트 대상), WaitForMutipleObjects(다수의 커널 오브젝트 대상) 함수를 통해서 Signaled 상태를 확인 가능
	// Linux에서는 pthread_join() 함수를 통해서 쓰레드의 종료 여부를 확인
	// 다만 다른 점은 pthread_join() 함수는 쓰레드의 종료와 함께 소멸까지 처리
	// Windows에서는 CloseHandle을 통해 쓰레드의 Usage Count를 감소시켜줘야 한다.
	WaitForMultipleObjects(
		NUM_THREAD, // 관찰할 커널 오브젝트의 수
		tHandles, // 커널 오브젝트가 담긴 배열
		TRUE, // TRUE로 설정하면 모든 오브젝트가 Signaled 상태일 때 반환, FALSE로 설정하면 하나라도 Signaled 상태가 되면 반환
		INFINITE // 계속 대기 (Blocking)
	);
	//DeleteCriticalSection(&cs);
	cout << "result: " << num << '\n';
	// 엄밀히 따지면 CloseHandle을 통해 쓰레드의 소멸을 확실하게 해야함
	for (DWORD i = 0; i < NUM_THREAD; i++)
		CloseHandle(tHandles[i]);
	return 0;
}

// _beginthreadex()에서 요구하는 함수의 양식이 다음과 같음
// unsigned (__stdcall* _beginthreadex_proc_type)(void*);
// WINAPI == CALLBACK == __stdcall
// 함수 호출 규약(Calling Convention)
// stdcall의 경우 호출자가 아닌 피호출자(함수)쪽에 스택 프레임 정리하는 코드가 생성
unsigned WINAPI threadInc(void* arg)
{
	//EnterCriticalSection(&cs);
	for (DWORD i = 0; i < 50000000; i++)
		InterlockedIncrement64(&num);
	//LeaveCriticalSection(&cs);
	return 0;
}

unsigned WINAPI threadDes(void* arg)
{
	//EnterCriticalSection(&cs);
	for (DWORD i = 0; i < 50000000; i++)
		InterlockedDecrement64(&num);
	//LeaveCriticalSection(&cs);
	return 0;
}