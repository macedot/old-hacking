#include <stdio.h>
#include <utmp.h>
#include <lastlog.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <shadow.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define UTMP "/etc/utmp"
#define UTMP2 "/var/run/utmp"
#define WTMP "/etc/wtmp"
#define WTMP2 "/var/log/wtmp"
#define LASTLOG "/var/log/lastlog"
#define SIM 1
#define NAO 0
#define MAX 14
#define NL putchar('\n')
#define BEEP putchar(7)

typedef char boolean;

struct user{
       struct utmp u;
       struct user *p;
       }*s;

struct utmp cmp;
struct lastlog cmp2;

void insere(struct utmp *);
void print(struct user *);
void ajuda(const char *);
void print_wtmp(struct user *);
boolean procura(struct user *, const char *);
boolean bash_history(const char *);
void never_login(const char *, const char *);

void main(int argc, char *argv[]){

struct utmp actual;
struct lastlog actual2;
struct spwd *utilizador;
struct passwd *username;

boolean intrusao=NAO,
        resto=SIM,
        beep1=NAO;
int intrusoes=0,
    i;

int fd;

time_t hora;
  
  for(i=0; i<argc; i++)
     if(argv[i][0]=='-')
       switch(argv[i][1]){
       case 'h':{
                 ajuda(argv[0]);
                 exit(0);
                }
       case 'b':{
                 beep1=SIM;
		 break;
                }
       case 'u':{
                 resto=NAO;
                 break;
                }
       default:{
                puts("Opcao desconhecida");
                exit(1);
                }
       }


  s=NULL;
  memset(&cmp, 0, sizeof(struct utmp));    
  memset(&cmp2, 0, sizeof(struct lastlog));   

  if((fd=open(UTMP, O_RDONLY))<0){
    if((fd=open(UTMP2, O_RDWR))<0){
      perror("utmp");
      exit(1);
      }
     }
 
  time(&hora);
  printf("%s\n", ctime(&hora));
 
  printf("Analizando entradas do 'utmp'...\n");
  while(read(fd, &actual, sizeof(actual))>0){
    if(!memcmp(&actual, &cmp, sizeof(struct utmp)))
      intrusao=SIM;
      insere(&actual);
  }
  

  
  if(intrusao){
    NL;
    if(beep1)
      BEEP;
    printf("**********\n");
    printf("* AVISO: * As entradas do 'utmp', foram manipuladas!!!!!\n");
    printf("**********\n");
    printf("Users correctamente logged on:\n");
    printf("User\t\tLogged on em\tdev\n");
    print(s);
    printf("Faca 'ps -aux' para procurar processos de users ilegais!\n");
    } 
  else
   puts("OK!");
  
  if(!resto)
    exit(0);
 
  printf("----------------------------------\n");

  if((fd=open(WTMP, O_RDONLY))<0)
    if((fd=open(WTMP2, O_RDONLY))<0){
      perror("wtmp");
      exit(1);
    }
  
  NL;

  printf("Analizando entradas do 'wtmp'...\n");
    
  s=NULL;
  while(read(fd, &actual, sizeof(struct utmp))>0){
    if(!memcmp(&actual, &cmp, sizeof(struct utmp)))
        intrusoes++;
        insere(&actual);
  }
 
     
  if(intrusoes!=NAO){
    printf("Foram 'apagadas' um total de %d entradas!\n\n", intrusoes);
    print_wtmp(s);
    }
   else
    puts("OK!");
   

  printf("----------------------------------\n");
  
  if(getuid()!=0)
    if(fopen("/etc/shadow", "r")==NULL){
      printf("Precisa ser root para continuar!!\n");
      exit(1);
      } 

  if((fd=open(LASTLOG, O_RDWR))>=0){
    printf("Analizando entradas do 'lastlog'...\n");
    intrusoes=0;
 
    puts("Utilizadores com entradas 'apagadas':");
    while((utilizador=getspent())!=NULL){
      if(utilizador->sp_pwdp[0]!='*'){
        username=getpwnam(utilizador->sp_namp);
        lseek(fd, username->pw_uid * sizeof(struct lastlog), SEEK_SET);
        if(read(fd, &actual2, sizeof(struct lastlog))==-1){
          perror(LASTLOG);
          break;
	  }
        if(!memcmp(&cmp2, &actual2, sizeof(struct lastlog))){
          if(procura(s, utilizador->sp_namp)){
            intrusoes++;
            printf("-> %s\n", utilizador->sp_namp);
	    }
          else
           if(bash_history(username->pw_dir)){
             intrusoes++;
             printf("-> %s\n", utilizador->sp_namp);
	     }
	   else
	     never_login(utilizador->sp_namp, username->pw_dir);
	  }
       }
   }
}
else
  {
   printf("Nao foi possivel localizar o ficheiro 'lastlog'\n");
   exit(1);
  }

  if(intrusoes!=NAO)
    printf("\nForam apagadas pelo menos %d entradas\n", intrusoes);
    else
       puts("\nOK!");
             
}

void insere(struct utmp *k){
struct user *novo;

  if((novo=(struct user *) malloc(sizeof(struct user)))==NULL){
    puts("Erro a alocar memoria!");
    exit(1);
    }

  memcpy(&novo->u, k, sizeof(struct utmp));
  novo->p=s;
  s=novo;
}


void print(struct user *x){

  if(x==NULL)
    return;

  if(!memcmp(&x->u, &cmp, sizeof(struct utmp)))
    printf("??????\t\t??????\t\t??????\n");
  else
   if(x->u.ut_type >=7 && x->u.ut_type <=8)
     printf("%s\t\t%s\t\t%s\n", x->u.ut_user, x->u.ut_host, x->u.ut_line);
 
  print(x->p);

}

void print_wtmp(struct user *x){

   if(x==NULL)
     return;

   if(!memcmp(&x->u, &cmp, sizeof(struct utmp))){
    printf("entrada 'apagada' depois de:\n");
    if(memcmp(&x->p->u, &cmp, sizeof(struct utmp))!=0){
     printf("%s", ctime(&x->p->u.ut_time));
     if(x->p->u.ut_type >=7 && x->p->u.ut_type <=8)
       printf("(%s)\n", x->p->u.ut_name);
    }
    else
     printf("????????????????????????\n");

     NL; 
   }

   print_wtmp(x->p);
}

void ajuda(const char *s){
printf("======RwX Labs 1998=====\n");
printf("Uso: %s [-opcoes]\n", s);
printf("        -h imprime este help e sai\n");
printf("        -b beep ao encontrar anomalias\n");
printf("        -u apenas testa utmp\n");

}

boolean procura(struct user *x, const char *s){

   if(x==NULL)
     return(NAO);

   if(!strcmp(s, x->u.ut_name))
     return(SIM);

   procura(x->p, s);
}


boolean bash_history(const char *s){
static char *dir;

  if((dir=(char *) malloc(strlen(s)+14))==NULL){
    puts("Erro a alocar memoria!!");
    exit(1);
    }

  sprintf(dir, "%s/.bash_history", s);

  if(fopen(dir, "r")==NULL)
    return(NAO);

    return(SIM);

} 

void never_login(const char *s, const char *dir){

printf("?? %s\n", s);
printf("nao possui nenhuma entrada em 'wtmp'\n");
printf("o arquivo '%s/.bash_history' nao existe\n", dir);
printf("provavelmente nunca usou o sistema!!\n");

}













