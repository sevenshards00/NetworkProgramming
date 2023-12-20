/*
* TCP/IP Socket Programming - 도메인 이름(Domain Name)과 인터넷 주소(IP Address)
* 파일명: gethostbyname.c
* 파일 버전: 0.1
* 작성자: Sevenshards
* 작성 일자: 2023-12-20
* 이전 버전 작성 일자:
* 버전 내용: Linux 기반 도메인 이름을 통한 IP 주소 가져오기(gethostbyname)
* 이전 버전 내용:
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>

void error_handling(const char* err_msg);

int main(int argc, char* argv[])
{
    int i;
    struct hostent* host;
    char addr[20];
    if(argc != 2)
    {
        printf("Usage: %s <Hostname>\n", argv[0]);
        exit(1);
    }

    host = gethostbyname(argv[1]);
    if(!host)
        error_handling("can't find host");
    
    printf("Official name: %s\n", host->h_name);
    for(i = 0; host->h_aliases[i]; i++)
        printf("Aliases %d: %s\n", i+1, host->h_aliases[i]);
    printf("Address type: %s\n", (host->h_addrtype == AF_INET) ? "AF_INET" : "AF_INET6");
    for(i = 0; host->h_addr_list[i]; i++)
    {
        // 이상한 부분에서 헤메고 있었다.
        // h_addr_list[i]는 char* -> 즉 IP 주소
        // 단, 네트워크 바이트 순서로 되어 있으므로 이를 바꿔줘야 함
        inet_ntop(host->h_addrtype, host->h_addr_list[i], addr, sizeof(addr));
        printf("IP Addr %d: %s\n", i + 1, addr);
    }
    return 0;
}

void error_handling(const char* err_msg)
{
	fputs(err_msg, stderr);
	fputc('\n', stderr);
	exit(1);
}