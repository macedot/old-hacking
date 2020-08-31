/*********************************************************************
     RWX 6
     CopyRight 1998 (R) ErAsEr  eraser@linuxbr.com.br
     Todos os direitos reservados
*********************************************************************/

#include <stdio.h>
#include <stdlib.h>

long long C,b,n,e;

long long modulo(long long a,long long e,long long n)
{
    unsigned long long A,E,P=1;
    A=a;
    E=e; 
etapa2:    
    if (E==0)
    {
       if (P>n) P=P%n;
       return P;
    }
    
    if ((E % 2)==1) /* se eh for impar */
    {
       P=((A % n) * (P % n));
       E=(E-1)/2;
    }
    else
    {  
       if ((E % 2)==0) E=E/2;
    }
    
    A=((A % n) * (A % n));
    
    goto etapa2;
}           

/* void main()
{
    printf("Digite o numero que voce quer codificar, o E e N : ");
    scanf("%lld %lld %lld",&b,&e,&n);
    C=modulo(b,e,n);
    printf("O numero codificado eh : %lld \n",C);
} */

void main()
{
   FILE *entrada;
   FILE *saida;
   char nome[80];
   char *nomesaida="codificado";
   printf("Digite o nome do arquivo : ");
   scanf("%s",nome);
   printf("Digite E e N : ");
   scanf("%lld %lld",&e,&n);
   entrada=fopen(nome,"r+b");
   if (!entrada)
   {
      printf("Erro ao tentar abrir o arquivo de entrada.\n");
      exit(1);
   }    
   saida=fopen("codificado","w+b");
   if (!saida)
   {
      printf("Erro ao tentar abrir o arquivo codificado.\n");
      exit(1);
   }    
   while (!feof(entrada))
   {
      short R;
      fread(&R,2,1,entrada);
      C=modulo((long long)R,e,n); 
      /* C=(long long)R; */
      fwrite(&C,8,1,saida);
   }
   fclose(entrada);
   fclose(saida);
} 
