/*  extract.c by Phrack Staff and sirsyko 
 *
 *  Phrack Magazine, 1997 
 *
 *  Extracts textfiles from a specially tagged flatfile into a hierarchical 
 *  directory strcuture. Use to extract source code from any of the articles 
 *  in Phrack Magazine (first appeared in Phrack 50).
 *
 *  gcc -o extract extract.c
 *  
 *  ./extract filename   
 * 
 */   

 
#include <stdio.h>
#include <sys/stat.h>
#include <string.h>

int main(int argc, char **argv){ 

    char *s="<++> ",*e="<-->",b[256],*bp; 
    FILE *f,*o = NULL; 
    int l, n, i=0; 

    l = strlen(s); 
    n = strlen(e); 

    if(argc<2) {
        printf("Usage: %s <inputfile>\n",argv[0]);
        exit(1); 
    }

    if(! (f=fopen(argv[1], "r"))) {
        printf("Could not open input file.\n");
	exit(1);
    }

    while(fgets(b, 256, f)){ 

        if(!strncmp (b, s, l)){ 
	    b[strlen(b)-1] = '\0'; 

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
        else if(!strncmp (b, e, n)){
	    if(o) fclose(o);
	    else {
	        printf("Error closing file.\n");
		exit(1);
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
