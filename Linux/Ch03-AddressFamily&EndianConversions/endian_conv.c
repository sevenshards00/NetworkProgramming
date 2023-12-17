/*
* TCP/IP Socket Programming - 주소체계와 데이터 정렬
* 파일명: endian_conv.c
* 파일 버전: 0.1
* 작성자: Sevenshards
* 작성 일자: 2023-12-17
* 이전 버전 작성 일자:
* 버전 내용: Host Ordered(little endian), Network Ordered(big endian) 변환 예제
* 이전 버전 내용:
*/

#include <stdio.h>
#include <arpa/inet.h>

int main(int argc, char* argv[])
{
	unsigned short host_port = 0x1234;
	unsigned short net_port;
	unsigned long host_addr = 0x12345678;
	unsigned long net_addr;
	
	// little endian -> big endian
	net_port = htons(host_port); // host to network short (16bit, 2Bytes)
	net_addr = htonl(host_addr); // host to network long (32bit, 4Bytes)
	
	printf("Host ordered port: %#x\n", host_port);
	printf("Network ordered port: %#x\n", net_port);
	printf("Host ordered address: %#lx\n", host_addr);
	printf("Network ordered address: %#lx\n", net_addr);
	
	return 0;
}
