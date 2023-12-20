/*
* TCP/IP Socket Programming - 도메인 이름(Domain Name)과 인터넷 주소(IP Address)
* 파일명: gethostbyaddr.c
* 파일 버전: 0.1
* 작성자: Sevenshards
* 작성 일자: 2023-12-20
* 이전 버전 작성 일자:
* 버전 내용: Linux 기반 주소를 통한 도메인 이름 가져오기(gethostbyaddr)
* 이전 버전 내용:
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>

void error_handling(const char* err_msg);

int main(int argc, char* argv[])
{
    int i;
    struct hostent* host;
    struct sockaddr_in addr;
    char ip_addr[20];
    if(argc != 2)
    {
        printf("Usage: %s <IP>\n", argv[0]);
        exit(1);
    }

    memset(&addr, 0, sizeof(addr));
    addr.sin_addr.s_addr = inet_addr(argv[1]);
    host = gethostbyaddr((char*)&addr.sin_addr, sizeof(addr.sin_addr.s_addr), AF_INET);
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
        inet_ntop(host->h_addrtype, host->h_addr_list[i], ip_addr, sizeof(ip_addr));
        printf("IP Addr %d: %s\n", i + 1, ip_addr);
    }
    return 0;
}

void error_handling(const char* err_msg)
{
	fputs(err_msg, stderr);
	fputc('\n', stderr);
	exit(1);
}