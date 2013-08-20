
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <sys/time.h>

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

#define FRAGMENT_MAX_SIZE 1400
int main(int argc, char *argv[]) {
    int       list_s;                /*  listening socket          */
    int       conn_s;                /*  connection socket         */
    short int port;                  /*  port number               */
    struct    sockaddr_in servaddr;  /*  socket address structure  */
    char      buffer[FRAGMENT_MAX_SIZE];      /*  character buffer          */
    char     *endptr;                /*  for strtol()              */
    char      ipaddr[100] = {0};
	enum CommandEnum { CommandUnknown, CommandCiAOListen, CommandCiAOSend } cmd = CommandUnknown;

    /*  Get host+port number+RECEIVE|SEND from command line */
    if ( argc == 4 ) {
		// port
		port = strtol(argv[2], &endptr, 0);
		if ( *endptr ) {
			fprintf(stderr, "Speedtest: Invalid port number.\n");
			exit(EXIT_FAILURE);
		}
		// ip
		strncpy(ipaddr, argv[1], strlen(argv[1]));
		// cmd
		if (strcmp(argv[3], "RECEIVE")==0) {
			printf("Test receive speed (CiAO TCP/IP send mode)\n");
			cmd = CommandCiAOSend;
		} else
		if (strcmp(argv[3], "SEND")==0) {
			printf("Test sending speed (CiAO TCP/IP receive mode)\n");
			cmd = CommandCiAOListen;
		} else {
			printf("The fourth argument has to be RECEIVE or SEND. You set: %s\n", argv[3]);
			exit(EXIT_FAILURE);
		}
	} else {
		printf("Usage: speedtest 10.0.3.2 1234 RECEIVE|SEND.\n");
		exit(EXIT_FAILURE);
    }

	/* Open socket */
    int sockfd, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");
    server = gethostbyname(ipaddr);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(port);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");

	/* Initialize time measure variables and transfer volumen */
	struct timeval start, end;
    long mtime, seconds, useconds;    
    gettimeofday(&start, NULL);
	#define TRANSFERSIZE           (1024*1024*100) // 100 MB

	// send data to ciao
	if (cmd == CommandCiAOListen) {
		// we send a tcp fragment with "LISTEN" as content to set CiAO IP to listen state //
		const char* cmdstr = "LISTEN";
		write(sockfd,cmdstr,sizeof(cmdstr));
		printf ("Start: ");
		for (int i=0;i<TRANSFERSIZE;i+=1024) {
			n = write(sockfd,buffer,FRAGMENT_MAX_SIZE);
			if (n < 0) 
				error("ERROR writing to socket");
			printf (".");
		}
		printf ("finished\n");
	}
	
    else if (cmd == CommandCiAOSend) {
		// we send a tcp fragment with "SEND" as content to set CiAO IP to send state //
		const char* cmdstr = "SEND";
		write(sockfd,cmdstr,sizeof(cmdstr));
		printf ("Start: ");
		for (int i=0;i<TRANSFERSIZE;i+=1024) {
			n = read(sockfd,buffer,FRAGMENT_MAX_SIZE);
			if (n < 0) 
				error("ERROR reading from socket");
			else if (n<FRAGMENT_MAX_SIZE)
				break;
			printf (".");
		}
		printf ("finished\n");
	}
	
    gettimeofday(&end, NULL);

    seconds  = end.tv_sec  - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;

    mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;

    printf("%u MB in %ld milliseconds\n", TRANSFERSIZE/1024/1024, mtime);
	double v = TRANSFERSIZE/1024/1024; //MB
	v /= (mtime/1000); // per second
	printf("Average speed: %f\n", v);

    close(sockfd);
    return 0;
}