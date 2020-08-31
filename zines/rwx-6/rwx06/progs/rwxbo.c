/*                ------------------------------
                       RWX PROUDLY PRESENTS:
                  ------------------------------
               RwXBO v. 0.0.0.2 beta 127.0.0.1
Now using syslog!

RWX Back Orifice Sweep Scanner

Authors: [0x5002](xsatanic@rwx.ml.org) 'n KByte(kbyte@rwx.ml.org)
Help by lrk0rx
[ Issues, Programs, Mail List => http://rwx.ml.org ]

RWXBO eh um programa simples nascido da ide'ia de pegar os engracadinhos
que ficam destruindo o computador de pobres mulheres indefesas.

*/
#include 	<syslog.h>
#include        <sys/types.h>
#include        <sys/socket.h>
#include        <netinet/in.h>
#include        <arpa/inet.h>
#include        <stdio.h>
#include        <stdlib.h>
#include        <string.h>
#include        <unistd.h>

#define  DIR      0
#define  INFO     1
#define  HTTPON80 2

char scanstr[1024]="Œc—“Áœ9••ÜMä¥f™2";
char dir[1024]="Œc—“Áœ<••Ü";
char info[1024]="Œc—“Áœ8••Ü";
char httpon80[1024]="Œc—“Áœ>";

char buffer[1024]="Œc—“Áœ••ÜºuKô™xƒ„÷¶eQuQÎ*?Ì0|[à—Êß¡ù";

char *host;

int main()					
{
	struct sockaddr_in	serv, cli;
	char			request[400], reply[400];
	int			sockfd, n, clilen;
      if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
		perror("socket error");

        openlog("rwxbo", LOG_PID|LOG_CONS, LOG_WARNING);

	memset(&serv, 0, sizeof(serv));

	serv.sin_family      = AF_INET;
	serv.sin_addr.s_addr = htonl(INADDR_ANY);
	serv.sin_port        = htons(31337);

	if (bind(sockfd, (struct sockaddr_in *) &serv, sizeof(serv)) < 0)
                 perror("bind");

        while(fork()==0) {
	for ( ; ; ) {
		clilen = sizeof(cli);
		if ( (n = recvfrom(sockfd, request, 400, 0,
				   (struct sockaddr_in *) &cli, &clilen)) < 0)
			perror("fake reply");
        
        host = inet_ntoa(cli.sin_addr);
        
        if (strstr(request,scanstr)) 
           syslog(LOG_WARNING, "-----(%s) is scanning you------\n", host);

        if (strstr(request,dir))  
           syslog(LOG_WARNING, "RwXBO:%s>dir\n", host);

        if (strstr(request,info)) 
           syslog(LOG_WARNING, "RwXBO:%s>passes or info\n", host);

        if (strstr(request,httpon80)) 
           syslog(LOG_WARNING, "RwXBO:%s>httpon 80\n", host);

        fflush(stdout);

 sendto(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&cli,sizeof(cli));
}
}
}
