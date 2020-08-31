/*********************************************************************
     RWX 6
     CopyRight 1998 (R) ErAsEr  eraser@linuxbr.com.br
     Todos os direitos reservados
*********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

long long p,q;
long long phi;

long long GCD(long long e,long long phi)
{
    long long res,a,PHI,E;
    E = e;
    PHI = phi;
    if (E > PHI)
    {
       while ((E % PHI) != 0)
       {
          a = E % PHI;
          E = PHI;
          PHI = a;
       }
       res = PHI;
    } 
    else      
    {
       while ((PHI % E) != 0)
       {
          a = PHI % E;
          PHI = E;
          E = a;
       }
       res = E;  
    }
    return res;
}        

long long achaE(long long phi,long long p,long long q)
{
   long long res=0;
   long long e=2;
   while (res != 1)
   {
      e++;
      res = GCD(e,phi);
   }
   return e;
}       

/* Essa funcao acha o inverso de e modulo phi usando o algoritmo Euclediano
   Extendido, */
long long extendido(long long e,long long phi) 
{
    long long u1 = 1;
    long long u2 = 0;
    long long u3 = phi;
    long long v1 = 0;
    long long v2 = 1;
    long long v3 = e;
    long long t1,t2,t3,q,uu,vv,z,inverso;    

    while (v3 != 0) 
    {
       q = u3/v3;
       t1 = u1 - q * v1;
       t2 = u2 - q * v2;
       t3 = u3 - q * v3;

       u1 = v1;
       u2 = v2;
       u3 = v3;

       v1 = t1;
       v2 = t2;
       v3 = t3;
       z = 1;
    }
    uu = u1;
    vv = u2;

    if (vv < 0) 
    {
       inverso = vv + phi;
    } else 
    {
       inverso = vv;
    }
    return inverso;
}    

   
   

void main()
{
    long long e;
    printf("Digite dois primos : ");
    scanf("%lld %lld",&p,&q);
    phi=(p-1)*(q-1);
    e = achaE(phi,p,q);
    printf("phi : %lld ; e : %lld ; d : %lld \nn : %lld\n",phi,e,extendido(e,phi),p*q);
    printf("Evite usar chaves maiores que 2**32 \n");
}     
