/* the subnetting program */
/* working version of a Class B subnetter falken@rune.org */
/* Werd silitek */

#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netdb.h>
FILE *fp;
void main(int argc,char *argv[]) {
int counter1=0;
int counter2=0;
if(argc!=3) {
printf("Sintaxe : %s ClassC_IP Arquivo_Out\n", argv[0]);
exit(0);
}
if((fp=fopen(argv[2], "at"))==0) {
printf("\nCant Open File ..\nQuitting...\n");
exit(0);
}
while(counter2 < 256)
                   {
                       counter1++;
                       if (counter1 == 255)
                           { counter2++;counter1=0;};

                  fprintf(fp, "%s.%i.%i\n",argv[1],counter2,counter1);
                   };
                };

