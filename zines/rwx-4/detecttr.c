/*
 * detecttr.c - by whynot AKA baldor (whynot@cyberjunkie.com)
 * created: 08.05.97
 * last modified: 11.07.97
 * Platforms: Linux, FreeBSD should work with other POSIX-systems too.
 *
 * Compile:
 * just the usual "gcc -o detecttr detecttr.c" for GNU C and 
 * "cc -o detecttr detecttr.c" for other compilers...
 * 
 * Usage:
 * Just run this program at the startup of your machine - it will stay in
 * the background until someone traceroutes you. It only uses a *tiny* bit
 * of your memory and nearly 0% CPU :)
 * 
 */


#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/signal.h>
#include <sys/syslog.h> /* simply comment this out if you don't have syslog.h */
#include <netdb.h>


#define MAXBUFLEN 200
#define MYPORT 33435
#define NUMPORTS 30*3

int sockfd[NUMPORTS];


void shutitdown()
{

	int w;
	char buf[50];
        for (w=0; w<NUMPORTS; w++)
	    close(sockfd);
        sprintf (buf,"DetectTraceroute terminated\n");
	syslog(LOG_NOTICE , buf);
	
	exit(0);
}

char *getname (struct in_addr addr)
{
	struct hostent *h;
	int w;
	char foo[4];   /* the 4 numbers as ASCII-Values per char */
	int tmpint[4]; /* used to convert from a string to 4 numbers */
	char tmpbuf[20];
	
	sprintf(tmpbuf, "%s", inet_ntoa(addr));

	if ( sscanf(tmpbuf,"%d.%d.%d.%d", &tmpint[0], &tmpint[1], &tmpint[2], &tmpint[3])  != 4) {
		printf ("Error while detecting hostname !\n");
		exit(1);
	}

	for(w=0; w<4; w++) foo[w]=tmpint[w];
	
	if ( (h=gethostbyaddr(foo, 4, AF_INET)) == NULL) {
		herror("gethostbyaddr");
		exit(1);
		
	}
	return  h->h_name;
}

main(int argc, char *argv[])
{
	int hops;
	struct sockaddr_in my_addr;
	struct sockaddr_in remote_addr;
	int addr_len, numbytes;
	char buf[MAXBUFLEN];
	int w;
	fd_set readfds;
	

	if( fork() !=0 ) return(0); /* we don't want to use that annonying & */
	
	
	signal(SIGHUP, SIG_IGN); /* ignore SIGHUP */
	
	signal(SIGTERM, shutitdown); /* clean shutdown */
	
	for(w=0; w<NUMPORTS; w++) {
		
		if ( (sockfd[w] = socket( AF_INET, SOCK_DGRAM, 0)) == -1) {
			perror("socket");
			exit(1);
			
		}
		my_addr.sin_family = AF_INET;
		my_addr.sin_port   = htons (MYPORT+w);
		my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	
		bzero(& (my_addr.sin_zero), 8);
   
		if ( bind (sockfd[w], (struct sockaddr *)&my_addr, sizeof (struct sockaddr) ) == -1) {
			perror("bind");
			exit(1);
		}
	}

	FD_ZERO(&readfds);
	for(w=0; w<NUMPORTS; w++)
	    FD_SET(sockfd[w], &readfds);

	
        sprintf (buf,"DetectTraceroute successfully started\n");
	syslog(LOG_NOTICE , buf);

	while(1) {
		select(sockfd[NUMPORTS-1]+1, &readfds, NULL, NULL, NULL);
		
		for (w=0; w < NUMPORTS; w++) {
			if (FD_ISSET(sockfd[w], &readfds))
			    hops = w;
		}
		
		addr_len = sizeof(struct sockaddr);
		
		if ((numbytes=recvfrom(sockfd[hops], buf, MAXBUFLEN, 0, (struct sockaddr *)&remote_addr, &addr_len)) == -1) {
			perror("recvfrom");
			exit(1);
		}
		
		/* we use buf for misc stuff  O:-) */
		sprintf (buf,"TRACEROUTE from IP %s. Hostname: %s  HOPS: %d", inet_ntoa(remote_addr.sin_addr), getname(remote_addr.sin_addr), hops / 3 +1);
		syslog(LOG_NOTICE , buf);
		FD_ZERO(&readfds);                   
		for(w=0; w<NUMPORTS; w++)
		    FD_SET(sockfd[w], &readfds);
		
	}
}
