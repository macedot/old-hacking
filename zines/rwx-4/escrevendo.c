/*
 Programa : Eu escrevo no Socket ( ehhehehe )
 Programador : rwx( knight )
 Compialar : gcc -o escrevendo escrevendo.c
 Descricao : Usa as funcoes send & write
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
char variavelchar1[80];
char variavelchar2[80];


// Menu Principal

void main(int argc, char *argv[])
{
   struct sockaddr_in sin;
   struct hostent *he;

   if(argc < 2) {
      printf("#rwx# EsCrEvendO nO sOCKET #rwx#\n");
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
   
   printf("Tudo OK -> Connectamos em [%s]\n",argv[1]);
   sleep(1); 
   printf("Gravando em variavelchar1 , USER ROOT lembra??\n");
   sprintf(variavelchar1, "USER root\n");
   sleep(1);
   printf("Gravando em variavelchar2, PASS ROOT hehe lembra disso??\n");
   sprintf(variavelchar2, "PASS root\n");
   sleep(1);
   printf("Pronto tudo gravado agora vamos escrever no socket\n");
   write(sockdestino, variavelchar1, strlen(variavelchar1));
   printf("Usamos o Write agora iremos usar o send\n");
   sleep(1);
   send(sockdestino, variavelchar2, strlen(variavelchar2),0);
   sleep(1);
   printf("Tudo Acabado Saindo :) ehhe\n");
   exit(0);
}
      
