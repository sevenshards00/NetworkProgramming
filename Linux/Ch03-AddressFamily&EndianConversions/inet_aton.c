/*
* TCP/IP Socket Programming - 주소체계와 데이터 정렬
* 파일명: inet_aton.c
* 파일 버전: 0.1
* 작성자: Sevenshards
* 작성 일자: 2023-12-17
* 이전 버전 작성 일자:
* 버전 내용: 문자열 정보를 네트워크 바이트 순서의 정수로 변환하는 예제(2)
* 이전 버전 내용:
*/

#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>

void error_handling(const char* message);

int main(int argc, char* argv[])
{
	const char* addr = "127.232.124.79";
	struct sockaddr_in addr_inet;
	
	// address to numeric
	// inet_addr과 마찬가지로 잘못된 ip 주소 형식에 대해 오류 검출이 가능
	if(!inet_aton(addr, &addr_inet.sin_addr))
		error_handling("Conversion Error Occured");
	else
		printf("Network ordered integer addr: %#x\n", addr_inet.sin_addr.s_addr);
		
	return 0;
}

void error_handling(const char* message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
