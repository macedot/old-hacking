#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/signal.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <errno.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>

void connect_server(char *host);

#define MASCARA     "vi                                       "
#define PORT  23   
#define MAX_HOST 64
#define SEG 5
#define MAX_BUF 64

#define IAC  255
#define DONT 254
#define DO   253
#define WONT 252
#define WILL 251


int sockdest;
char buffer1[MAX_BUF];
struct sockaddr_in cserv;
struct hostent *svr;
FILE *hostfile, *out;

void pn()
{
   return;
}

void main(int argc, char *argv[])
{
   char host[512];

   if(argc !=3) 
   {
      printf("\n\nuse: %s infile outfile\n\n\n",argv[0]);
      exit(0);
   }
   if((hostfile=fopen(argv[1],"rt"))==NULL) 
   {
      printf("Erro no %s\n", argv[1]);
      exit(0);
   }
   if((out=fopen(argv[2],"at"))==NULL) 
   {
      printf("Erro no %s\n", argv[2]);
      exit(0);
   }
   printf("\nWingate Seeker\n\n");
   fprintf(out, "Wingate Seeker by KByte & Mandarin\n");
   fprintf(out, "Lista de Wingates encontrados :\n");
   strcpy(argv[0], MASCARA);
   while(fgets(host, MAX_HOST , hostfile))
   {
         printf("Procurando %s", host);
         *strchr(host, '\n')='\0';
         connect_server(host);
   }
}

void connect_server(char *host)
{
   int ret;

   alarm(SEG);
   signal(SIGALRM, pn);
   if(!(svr =gethostbyname(host)))
      svr = gethostbyaddr(host, 15, AF_INET);
   if (svr)
   {
      memcpy((caddr_t)&cserv.sin_addr.s_addr, svr->h_addr, svr->h_length);
      if(sockdest < 0) 
      {
         printf("[ERRO] Serio Problema no Socket [ERRO]\n");
	 exit(0);
      }
      cserv.sin_family = AF_INET;
      cserv.sin_port = htons(PORT);
      sockdest = socket(AF_INET, SOCK_STREAM, 0);
      if (sockdest < 0)
      {
         printf("Socket nao aberto\n");
         return;
      }
      if(connect(sockdest, (struct sockaddr *)&cserv, sizeof(cserv)) < 0)
      {
         printf("porta fechada ou host nao responde\n");
      }
      else 
      {
         memset(buffer1, '\0', MAX_BUF);
         if ((ret = read(sockdest, buffer1, MAX_BUF))  < 0)
         {
            if (errno == SIGINT)
               fprintf(stderr, "Tempo esgotado\n");
            else
               fprintf(stderr, "Erro de leitura do socket ret = %d error = %d\n", 
                             ret, errno);
         }
         else
         {
            ret = read(sockdest, buffer1, MAX_BUF);
            if(!(memcmp(buffer1, "WinGate>", 8)) || 
               !(memcmp(buffer1, "Too man", 7))) 
	    {
	       fprintf(out, "%s\n", host);
	       printf("Wingate achado\n\a");
            } 
         }
      }
   } 
   else 
      printf("nao encontrado\n");
   close(sockdest);
}
