/*
        Marcus Ranum 1985
        usage: spy &
        the program will exit cleanly when you log out.
*/
#include <sys/types.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <ctype.h>
#include <utmp.h>


/*
 * Defines to work around System V
 * lameness.  sysvreallydoessux.
 */

#define bcopy(s1, s2, len) memcpy(s2, s1, len)
#define index(s, c) strchr(s, c)
#define rindex(s, c) strrchr(s, c)
#define bzero(s, len) memset(s, 0, len)
#define bcmp(s1, s2, len) (memcmp(s1, s2, len)!=0)
#define WAITIME 5


#define USIZ    (sizeof(struct utmp))

extern  char    *malloc();
extern  char    *realloc();
extern  off_t   lseek();

static  void
szprint(s,max)
char    *s;
int     max;
{
        /* catch the bastard even if they put esc. codes in ! */
        while(max-- && *s != '\0')
                if(isprint(*s))
                        (void)fprintf(stderr,"%c",*s++);
                else
                        (void)fprintf(stderr,"(0x%X)",*s++);
}


static  void
notify(old,new)
struct  utmp    *old;
struct  utmp    *new;
{
char    *op;
struct  utmp    *u;

        u=new;
        op="in";
        if(new->ut_name[0] == '\0') {
                u = old;
                op = "out";
        }

        szprint(u->ut_name,sizeof(u->ut_name));
        if(u->ut_line[0] != '\0') {
                (void)fprintf(stderr,"@");
                szprint(u->ut_line,sizeof(u->ut_line));
        }
        (void)fprintf(stderr," (");
        szprint(u->ut_line,sizeof(u->ut_line));
        (void)fprintf(stderr,") ( %DATE ) logged %s\n ",op);
}


main()
{
        time_t  m_time;
        int     utfd;
        struct  utmp    *up;
        struct  utmp    ubuf;
        int     upsiz;
        struct  stat    sbuf;

        if(stat(UTMP_FILE,&sbuf) != 0 || (utfd = open(UTMP_FILE,O_RDONLY)) < 0) {
                perror(UTMP_FILE);
                exit(1);
        }
        m_time = sbuf.st_mtime;

        if((up = (struct utmp *)malloc((unsigned)sbuf.st_size)) == NULL) {
                perror("malloc utmp buffer");
                exit(1);
        }
        upsiz = sbuf.st_size;

        if(read(utfd,(char *)up,(int)sbuf.st_size) != sbuf.st_size) {
                (void)fprintf(stderr,"read %ld bytes from ",sbuf.st_size);
                perror(UTMP_FILE);
                exit(1);
        }

        while(1) {
                if(getppid() == 1)
                        exit(0);

                (void)sleep(WAITIME);

                if(stat(UTMP_FILE,&sbuf) < 0) {
                        perror(UTMP_FILE);
                        exit(1);
                }

                if(upsiz < sbuf.st_size) {
                        up = (struct utmp *)realloc((char *)up,(unsigned)sbuf.st_size);
                        if(up == NULL) {
                                perror("realloc utmp buffer");
                                exit(1);
                        }
                        upsiz = sbuf.st_size;
                }

                if(sbuf.st_mtime != m_time) {
                        int     i;

                        if(lseek(utfd,0L,0) != 0L)
                                continue;

                        for(i = 0; i < sbuf.st_size / USIZ; i++) {
                                if(read(utfd,(char *)&ubuf,USIZ) != USIZ)
                                        continue;
                                if(bcmp((char *)&up[i],(char *)&ubuf,USIZ)) {
                                        notify(&up[i],&ubuf);
                                        (void)bcopy((char *)&ubuf,(char *)&up[i],USIZ);
                                }
                        }
                        m_time = sbuf.st_mtime;
                }
        }
}
