/*********************************************************************
     RWX 6
     CopyRight 1998 (R) ErAsEr  eraser@linuxbr.com.br
     Todos os direitos reservados
*********************************************************************/

#include <stdio.h>
#include <stdlib.h>

#define tabelamax 2000000

int tabela[tabelamax];

int achaproximozero(int c);

int numero;

void main()
{
   int i;
   int fim;
   int atual;
   int oldatual;
   int uns;

   for (i=0;i<=numero;i++)
   {
       tabela[i] = 0;
   }
   printf("Numero : ");
   scanf("%i",&numero);
   printf("\n Calculando....\n");   

   fim = 0;

   atual = 2;
   while (fim==0)
   {
      register int pont;
      pont = atual;
      while (pont<=numero)
      {
         int a;
         pont =pont + atual;
         tabela[pont] = 1;
      }
      tabela[0] = 1;
      pont=atual;
      pont++;
      while (tabela[pont]==1) pont++;
      atual=pont;
                  

      if (atual > numero) fim=1;

   }

   printf("Numeros : \n");
   for(i=0;i<=numero;i++)
   {
      if (tabela[i]==0) printf("%d\n",i);
   }   
}

int achaproximozero(int c)
{
   int fim;
   int i;
   fim = 0;
   i = c;
   while (fim==0)
   {
      i++;
      if (tabela[i] == 0) fim = 1;
   }
   return i;
}

int contazeros()
{
   int contador;
   int i;
   contador = 0;
   for (i=0;i<=numero;i++)
   {
      if (tabela[i] == 0) contador++;
   }
   return contador;
}
