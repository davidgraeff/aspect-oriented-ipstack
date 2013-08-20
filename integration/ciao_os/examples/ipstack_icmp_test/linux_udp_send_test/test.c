/*
 * gcc -o client test.c
 * ./client 10.0.3.2
 */
 
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h> 
#include <string.h>
#define BUFLEN 512
#define PORT 88
 
void err(char *s)
{
    perror(s);
    exit(1);
}
 
int main(int argc, char** argv)
{
    int sockfd;
    char buf[BUFLEN];
 
    if(argc != 2)
    {
      printf("Usage : %s <Server-IP>\n",argv[0]);
      exit(0);
    }
 
	
	struct sockaddr_in serv_addr;
	struct sockaddr_in6 serv_addr6;
	bzero(&serv_addr, sizeof(serv_addr));
	bzero(&serv_addr6, sizeof(serv_addr6));
	
    if (inet_pton(AF_INET, argv[1], &serv_addr.sin_addr)!=0) {
		
		serv_addr.sin_port = htons(PORT);
		if ((sockfd = socket(PF_INET6, SOCK_DGRAM, IPPROTO_UDP))==-1)
			err("socket");
		serv_addr.sin_family = AF_INET;
		printf("IPv4 address\n");
		if (sendto(sockfd, buf, BUFLEN, 0, (struct sockaddr*)&serv_addr, sizeof(serv_addr))==-1)
			err("sendto()");
		
	} else if (inet_pton(AF_INET6, argv[1], &serv_addr6.sin6_addr)!=0) {

		serv_addr6.sin6_port = htons(PORT);
		if ((sockfd = socket(PF_INET6, SOCK_DGRAM, IPPROTO_UDP))==-1)
			err("socket");
		serv_addr6.sin6_family = AF_INET6;
		printf("IPv6 address\n");
		if (sendto(sockfd, buf, BUFLEN, 0, (struct sockaddr*)&serv_addr6, sizeof(serv_addr6))==-1)
			err("sendto()");
    } else {
        fprintf(stderr, "Address not recognised. IPv4 and IPv6 addresses are valid\n");
        exit(1);
	}

	close(sockfd);
	return 0;
}