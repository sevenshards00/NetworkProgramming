/*
* TCP/IP Socket Programming - 주소체계와 데이터 정렬
* 파일명: inet_ntoa.c
* 파일 버전: 0.1
* 작성자: Sevenshards
* 작성 일자: 2023-12-17
* 이전 버전 작성 일자:
* 버전 내용: 네트워크 바이트 순서의 정수를 호스트 바이트 순서의 문자열로 변환하는 예제
* 이전 버전 내용:
*/

#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>

int main(int argc, char* argv[])
{
	struct sockaddr_in addr1, addr2;
	char* str_ptr;
	char str_arr[20];
	
	addr1.sin_addr.s_addr = htonl(0x01020304); // host to network long
	addr2.sin_addr.s_addr = htonl(0x01010101); // host to network long
	
	str_ptr = inet_ntoa(addr1.sin_addr);
	strcpy(str_arr, str_ptr);
	printf("Dotted-Decimal notation1: %s\n", str_ptr);
	
	inet_ntoa(addr2.sin_addr); // str_ptr = inet_ntoa(addr2.sin_addr);
	printf("Dotted-Decimal notation2: %s\n", str_ptr);
	printf("Dotted-Decimal notation3: %s\n", str_arr);
	
	return 0;
}
