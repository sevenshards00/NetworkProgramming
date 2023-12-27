/*
* TCP/IP Socket Programming - Windows에서의 쓰레드 동기화
* 파일명: SyncSema_win.cpp
* 파일 버전: 0.1
* 작성자: Sevenshards
* 작성 일자: 2023-12-28
* 이전 버전 작성 일자:
* 버전 내용: 커널 모드 동기화(2) - Semaphore 기반의 동기화
* 이전 버전 내용:
*/

#include <iostream>
#include <cstdio>
#include <Windows.h>
#include <process.h>
using std::cin;
using std::cout;

// unsigned == unsigned int
// Linux 버전의 semaphore.c와 동일한 구조를 Windows 기반으로 바꾼 코드
unsigned WINAPI Read(void* arg); // 증가 함수
unsigned WINAPI Accu(void* arg); // 감소 함수

static HANDLE semOne;
static HANDLE semTwo;
static int num;

int main(int argc, char* argv[])
{
	HANDLE hThread1, hThread2;
	semOne = CreateSemaphore(NULL, 0, 1, NULL);
	semTwo = CreateSemaphore(NULL, 1, 1, NULL);

	hThread1 = (HANDLE)_beginthreadex(NULL, 0, Read, NULL, 0, NULL);
	hThread2 = (HANDLE)_beginthreadex(NULL, 0, Accu, NULL, 0, NULL);

	WaitForSingleObject(hThread1, INFINITE);
	WaitForSingleObject(hThread2, INFINITE);

	CloseHandle(semOne);
	CloseHandle(semTwo);
	CloseHandle(hThread1);
	CloseHandle(hThread2);
	return 0;
}

unsigned WINAPI Read(void* arg)
{
	for (DWORD i = 0; i < 5; i++)
	{
		cout << "Input num: ";
		//fputs("Input num: ", stdout);
		WaitForSingleObject(semTwo, INFINITE);
		cin >> num;
		//scanf_s("%d", &num);
		ReleaseSemaphore(semOne, 1, NULL);
	}

	return 0;
}

unsigned WINAPI Accu(void* arg)
{
	DWORD sum = 0;
	for (DWORD i = 0; i < 5; i++)
	{
		WaitForSingleObject(semOne, INFINITE);
		sum += num;
		ReleaseSemaphore(semTwo, 1, NULL);
	}
	cout << "Result: " << sum << '\n';
	//printf("Result: %d\n", sum);
	return 0;
}