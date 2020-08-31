/*********************************************************************
     RWX 6
     CopyRight 1998 (R) ErAsEr  eraser@linuxbr.com.br
     Todos os direitos reservados
*********************************************************************/

#include <stdio.h>
#include <stdlib.h>

long long D,a,d,n;

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
   printf("Digite o numero criptografado, N e D : ");
   scanf("%lld %lld %lld",&a,&n,&d);
   D=modulo(a,d,n);
   printf("O numero decodificado eh : %lld \n ",D);
}*/



void main()
{
   FILE *entrada;
   FILE *saida;
   char nome[80];
   char *nomesaida="decodificado";
   printf("Digite o nome do arquivo : ");
   scanf("%s",nome);
   printf("Digite D e N : ");
   scanf("%lld %lld",&d,&n);
   entrada=fopen(nome,"r+b");
   if (!entrada)
   {
      printf("Erro ao tentar abrir o arquivo codificado.\n");
      exit(1);
   }    
   saida=fopen("decodificado","w+b");
   if (!saida)
   {
      printf("Erro ao tentar abrir o arquivo decodificado.\n");
      exit(1);
   }    

   while (!feof(entrada))
   {
      long long R;
      short S;
      fread(&R,8,1,entrada);
      D=modulo(R,d,n);
      S=(short)D;
      fwrite(&S,2,1,saida);
   }
   fclose(entrada);
   fclose(saida);
} 
