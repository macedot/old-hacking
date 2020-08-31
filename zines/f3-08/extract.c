/*  extract.c by Phrack Staff and sirsyko 
 *  modificado by Cheat Struck
 *  
 *
 *  Use este programa para extrair source codes da Phrack e Fatal 3rror.
 *  Use to extract source codes from Phrack Magazine and Fatal 3rror.
 *
 *  Para extrair todos os sources:
 *  To extract all sources:
 *    ./extract filename   
 *
 *  Para extrair apenas um source:
 *  To extract only one src:
 *    ./extract filename number
 *
 *  Para listar os sources:
 *  To list all sources:
 *    ./extract filename -list
 * 
 */   

 
#include <stdio.h>
#include <sys/stat.h>
#include <string.h>

int main(int argc, char **argv){ 

    char *s="<++> ",*e="<-->",b[256],*bp; 
    FILE *f,*o = NULL; 
    int l, n, i=0, list=0, count=0, numbah=0; 

    l = strlen(s); 
    n = strlen(e); 

    if(argc<2) {
        printf("Usage: %s <inputfile>\n",argv[0]);
        printf("       %s <inputfile> <number>\n",argv[0]);
        printf("       %s <inputfile> -list\n",argv[0]);
        exit(1); 
    }

    if(! (f=fopen(argv[1], "r"))) {
        printf("Could not open input file.\n");
	exit(1);
    }

    if (argc >= 3) {
       if (!strcmp(argv[2],"-list")) list++;
       else numbah=atoi(argv[2]);
    }

    while(fgets(b, 256, f)){ 

        if(!strncmp (b, s, l)){ 
          count++;
          b[strlen(b)-1] = '\0'; 

          if (list) printf("%d%s\n",count,b);
          else {
            if(!numbah || numbah==count) {

                if((bp=strchr(b+l+1,'/')))
                    while (bp){ 
                       *bp='\0';
		       mkdir(b+l, 0700); 
		       *bp='/';
		       bp=strchr(bp+1,'/'); 
		    }
                if((o = fopen(b+l, "w"))) 
	            printf("- Extracting %s\n",b+l);
	        else {
		    printf("Could not extract '%s'\n",b+l);
                    exit(1);
                }
	    }
          }
	} 
        else if(!strncmp (b, e, n)){
          if (!list) {
            if(!numbah || numbah==count) {

                if(o) fclose(o);
	        else {
	            printf("Error closing file.\n");
		    exit(1);
                }
	    }
          }
        } 
        else if(o) {
            fputs(b, o);
            i++;
        }
    }
    if(!i) printf("No extraction tags found.\n");
    return(0);
}
