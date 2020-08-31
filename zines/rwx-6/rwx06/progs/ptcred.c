/* 
OBS: UDP and ICMP not implemented | TODO:
-------------------------------------------------------------------
     TCP/UDP/ICMP Redirector -    | UDP Stuph
               September, 1998    | ICMP Stuph
                                  | Pop Corn
-------------------------------------------------------------------
TCP source by [0x5002]            
Other stuph by TYPe_0

Slakware/SuSE/RH4.X : $ gcc -D__resource__ -o ptcred ptcred.c
FreeBSD/RH5.X       : $ gcc -o ptcred ptcred.c
*/

#define PORTA 9999

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

/*
Se vc estiver compilar sob SlakWare/SuSE/RH4.X(?) use -D__resource__ na linha
de comando, se vc estiver em um RH5.0/5.1(glibc) ou FreeBSD, compile normalmente
*/

#ifdef __resource__
#include <sys/resource.h>
#endif

#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
#include <sys/ioctl.h>

char PROCESS_NAME[10] = "-bash"; /* Nome do Processo falso */

void telcli(int,char*,int);
void emulate(int,int);

int main(int argc,char *argv[]) {

  int srv_fd, rem_fd, len, port, len2, x;
  fd_set readit;
  char buff[100], nick[100],user[100], porta[100];

  struct sockaddr_in rem, srv;

  strncpy(argv[0],PROCESS_NAME,sizeof(PROCESS_NAME)); /* Fake Name */

  bzero((char *)&srv, sizeof(srv));
  bzero((char *)&rem, sizeof(rem));

  srv.sin_family = AF_INET;
  srv.sin_addr.s_addr = htonl(INADDR_ANY);
  srv.sin_port = htons(PORTA);
  srv_fd = socket(PF_INET, SOCK_STREAM, 0);

  bind(srv_fd, (struct sockaddr *) &srv, sizeof(srv));
  listen(srv_fd, 2);

  if (fork()) exit(0);
  while (1) {
    len = sizeof(rem);
    rem_fd=accept(srv_fd, (struct sockaddr *)&rem, &len);
    switch(fork()) {
       case 0:
          close(srv_fd);
          dup2(rem_fd,0);
          dup2(rem_fd,1);
          dup2(rem_fd,2);
          printf(":PTC.Redirector NOTICE AUTH :info "
                 "o. Welcome to PROTOCOL Redirector .o\n");

          printf(":PTC.Redirector NOTICE AUTH :info "
                 "WHERE SHOULD I CONNECT TO? \n");

          fflush(stdout);
          FD_ZERO(&readit);
          FD_SET(rem_fd,&readit);
          x = 0;
          bzero(buff,100);

          /*
             Isso poderia ser evitado usando o comando server ou connect
             mas to muito ocupado pra implementar essas coisa, quem tiver afim
             coloca e me manda 
          */

          while(x!=3) {
             len2=select(256,&readit,0,0,0);
             if (x == 0) len2=read(rem_fd,nick,sizeof(nick));
             if (x == 1) len2=read(rem_fd,user,sizeof(user));
             if (x == 2) {
                len2=read(rem_fd,buff,sizeof(buff));
                break;
             }
             x++;
          }

          buff[(strlen(buff)-2)] = '\0';

          printf(":PTC.Redirector NOTICE AUTH :host [%s]\n",buff);
          printf(":PTC.Redirector NOTICE AUTH :info What port?\n");
          fflush(stdout);
          len2=select(256,&readit,0,0,0);
          if (len2>0) len2=read(rem_fd,porta,sizeof(port));
          printf(":PTC.Redirector NOTICE AUTH :port [%d]\n",atoi(porta));
          fflush(stdout);
          telcli(rem_fd,buff,atoi(porta));
          close(rem_fd);
          exit(0);
          break;
       default:
	  close(rem_fd);
	  if (fork()) exit(0);
	  break;
       };
   }
}

void telcli(int source, char victim[1025], int port)
{
   int dest, found;
   struct sockaddr_in sa;
   struct hostent *host;
   char gethost[100];
   char getport[100];
   char string[200];
   host = gethostbyname(victim);

/* abertura de tty pra telnet... */

   if (port==23) {
      if ((dest = open("/dev/tty", O_RDWR)) >= 0) {
         ioctl(dest, TIOCNOTTY, (char *)0);
         close(dest);
      }
   }

   if (host) memcpy((caddr_t)&sa.sin_addr.s_addr, host->h_addr, host->h_length);

   sa.sin_family = AF_INET;
   sa.sin_port = htons(port);

   if ((dest = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
       perror("telcli: socket");
       exit(1);
   }

   connect(dest, (struct sockaddr *) &sa, sizeof(sa));
   emulate(source,dest);

/* No delay */

   fcntl(source,F_SETFL,O_NDELAY);
   fcntl(dest,F_SETFL,O_NDELAY);
   close(dest);
   exit(0);
}


void emulate ( int source, int dest) {
   fd_set wr, rd;
   int waitdata, dataread;

   struct SERVER {
      int position;
      char *head, *tail, buffer[2480];
   };

   struct SERVER server;
   struct SERVER client;
   
   client.head = client.tail = client.buffer;
   client.position = 0;
   server.head = server.tail = server.buffer;
   server.position = 0;

   /* Algoritimo do Telnet Client, o resto foi meu mermo */

   while(1) {
      FD_ZERO(&wr);
      FD_ZERO(&rd);

      if (server.position < sizeof(server.buffer)-1) FD_SET(dest, &rd);
      if (client.position < sizeof(client.buffer)-1) FD_SET(source, &rd);
      if (client.tail > client.head) FD_SET(dest, &wr);
      if (server.tail > server.head) FD_SET(source, &wr);
    
      waitdata = select(256, &rd, &wr, 0, 0);
      if (waitdata <= 0) continue;

      if (FD_ISSET(dest,&rd)) {
         dataread=read(dest,server.tail, sizeof(server.buffer)-server.position);
         if (dataread==0) return;
         if (dataread>0) {
            server.position += dataread;
            server.tail += server.position;
            if(!--waitdata) continue;
         }
      }
      if (FD_ISSET(source,&rd)) {
         dataread=read(source,client.tail, sizeof(client.buffer)-client.position);
         if (dataread==0) return;
         if (dataread>0) {
            client.position += dataread;
            client.tail += client.position;
            if(!--waitdata) continue;
        }
      }
     
     if (FD_ISSET(dest,&wr)) {
         dataread=write(dest,client.head,client.tail-client.head);
         if (dataread>0) {
             client.head += dataread;
             if (client.head == client.tail) {
                client.head = client.tail = client.buffer;
                client.position = 0;
             }
             if (!--waitdata) continue;
         }
   }

    if (FD_ISSET(source,&wr)) {
        dataread=write(source,server.head,server.tail-server.head);
        if (dataread>0) {
            server.head += dataread;
            if (server.head == server.tail) {
                server.head = server.tail = server.buffer;
                server.position = 0;
            }
            if (!--waitdata) continue;
        }
      }
   }
}
