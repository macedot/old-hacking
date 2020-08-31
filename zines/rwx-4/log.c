/* Mesmo que o log.sh so que em C */
#include <stdio.h>

void main(void) {
char comando[1024];
int i;
printf("core dumped\n");
printf("bash# ");
gets(comando);
sleep(5);
for(i=0;i<10;i++) {
printf("#8[?3;5h(0] [2J#8[?3;5h(0 **B00");
}
}
