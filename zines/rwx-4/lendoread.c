/*
 Programa : Eu Leio o Socket usando READ ( CRUZIS )
 Programador : rwx( knight )
 Compialar : gcc -o lendoread lendoread.c
 Descricao : Usa a funcao read
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
int sockdestino2; // Olha nossa variavel INTERGER ai :)
char euqueroler[4048]; // Olha nossa variavel ai


// Menu Principal

void main(int argc, char *argv[])
{
   struct sockaddr_in sin;
   struct hostent *he;

   if(argc < 2) {
      printf("#rwx# LenDo o SOcket #rwx#\n");
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
   sin.sin_port = htons(25);  // Porta do Sendmail
   
   if(connect( sockdestino, (struct sockaddr * )&sin, sizeof(sin)) < 0 ) {
      printf("Nao foi possivel Connectar nesta porta\n");
      exit(0);
   }
   
   printf("Vamos Ler -> Connectamos em [%s]\n",argv[1]);
   sleep(1); 
   printf("\n");
   sleep(1);
   printf("O que sera que vai acontecer????\n");
   sleep(1);
   printf("E um homem???? e um passaro????\n");
   sleep(1);
   printf("HEHEH e nada nao so o daemon sendmail :)\n");
   printf("\n");
   
   sockdestino2 = read(sockdestino, euqueroler, 4048); // Nossa funcao
   
   printf("Sendmail Daemon : %s\n", euqueroler);
   sleep(2);
   printf("\n");
   printf("Vamo nessa :)\n");
   exit(0);
}

