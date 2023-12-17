/*
* TCP/IP Socket Programming - 주소체계와 데이터 정렬
* 파일명: inet_addr.c
* 파일 버전: 0.1
* 작성자: Sevenshards
* 작성 일자: 2023-12-17
* 이전 버전 작성 일자:
* 버전 내용: 문자열 정보를 네트워크 바이트 순서의 정수로 변환하는 예제
* 이전 버전 내용:
*/

#include <stdio.h>
#include <arpa/inet.h>

int main(int argc, char* argv[])
{
	const char* addr1 = "1.2.3.4";
	const char* addr2 = "1.2.3.256"; // 잘못된 ip 주소 형식
	
	// addr1 convert
	unsigned long conv_addr = inet_addr(addr1);
	if(conv_addr == INADDR_NONE)
		printf("Error occured!\n");
	else
		printf("Network ordered integer addr: %#lx\n", conv_addr);
	
	// addr2 convert
	conv_addr = inet_addr(addr2);
	if(conv_addr == INADDR_NONE)
		printf("Error occured!\n");
	else
		printf("Network ordered integer addr: %#lx\n", conv_addr);
		
	return 0;
}
