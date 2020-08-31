/*
 Programa : Eu Leio o Socket usando FILE STREAM ( HHEHEHEHEHEHE )
 Programador : rwx( knight )
 Compialar : gcc -o lendostream lendostream.c
 Descricao : Usa a funcao fdopen
*/

// Includes files 
#include <stdio.h> 
#include <string.h>
#include <stdlib.h>
#include <netdb.h> 
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>


// Variaveis Globais
int sockdestino;
FILE *fd;
char euqueroler[4048]; // Olha nossa variavel ai
char escreve1[40]="USER root\n";
char escreve2[40]="PASS root\n";
char escreve3[40]="QUIT\n";
int i=0;
char temp;
// Menu Principal

void main(int argc, char *argv[])
{
   struct sockaddr_in sin;
   struct hostent *he;

   if(argc < 2) {
      printf("#rwx# LenDo o SOcket (FILE STREAM) #rwx#\n");
      printf("sintaxe : %s host\n",argv[0]);
      exit(0);
   }
   // Default 
   
   he=gethostbyname(argv[1]);
   
   if(he) {
      memcpy((caddr_t)&sin.sin_addr.s_addr, he->h_addr, he->h_length);
   } else {
      printf("Eu Nao Consegui Connectar em [%s]\n",argv[1]);
      exit(0);
   }
   
   sockdestino = socket(AF_INET, SOCK_STREAM, 0);
   
   if(sockdestino < 0) {
      printf("* Problema no Socket *\n");
      exit(0);
   }
   
   // Configuracao do Socket
   
   sin.sin_family = AF_INET;
   sin.sin_port = htons(110);  // Porta do POP3D
   
   if(connect( sockdestino, (struct sockaddr * )&sin, sizeof(sin)) < 0 ) {
      printf("Nao foi possivel Connectar nesta porta\n");
      exit(0);
   }
   
   printf("Vamos Ler -> Connectamos em [%s]\n",argv[1]);
   sleep(1); 
   printf("\n");
   if((fd=fdopen(sockdestino, "r")) == NULL ) {       // Olha nossa funcao
      printf("Cara.. ta com problema no Socket Stream :)\n");
      exit(0);
   }
   for(i=0;i<140;i++) {
      sizeof(temp);
      temp=fgetc(fd);
      fputc(temp, stdout);
   }
      close(sockdestino);
      close(fd);
      puts(""); 
      exit(0);
}

