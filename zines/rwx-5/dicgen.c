/* =[Jan/98]=================================
     Dictionary Generator II ver 1.0 (0.04a)
	by Eradicator/Ceara Ratz Org.
      Helped by ][MOSCA][ (thanks, man!).
	Copyright(C) by Eradicator/CR.
      Original desenvolvido em Pascal7.
   ========================================== */

#include <stdio.h>
#include <stdlib.h>

#define TELA true          // Opá∆o para n∆o aparecer as words geradas na
			   // tela para que o processo fique mais rapido.
			   // true = mostra / false = § mostra

#define D_Varis "1234567890\n"		     // Variaveis default
#define D_MinMax "0105\n"		     // caso o arquivo dicgen.cfg
					     // nao exista.

#define ver "1.00a"
#define erro1 "Error: Can't open config file (dicgen.cfg). Writing defaults.\n"
#define erro2 "Error: Can't open output file (%s).\n"
#define erro3 "Error: Invalid config file (dicgen.cfg).\n"
#define erro4 "Error: Can't write default config file (dicgen.cfg).\n"
#define erro5 "Error: Can't open default config file (dicgen.cfg).\n"

char stdoutx='f';         // Opá∆o que define como saida das words geradas
			   // para o stdout e nao para um arquivo.
char Varis[255];                     // Variaveis usadas
char Palavra[17];                    // Palavra enviada, c/ tam max 16 chars
char Min[3],Max[3];                  // Min e Max
unsigned long Bytes,Vezes;   	     // Dados estatisticos do dicionario
int  ia,ib,ic,id,ie,ig,ih,ii;        // Variavel Auxiliar nos Loops
int  ij,ik,il,im,in,io,ip,iq;        // +Vars de Auxilio
int  mk;			     // Var de auxilio do MkHlp();
int  loc=4;			     // Var pra determinar posicionamento na tela
FILE *config,*dic;                   // ConfigFile e Outputfile

void main (int argc, char *argv[]) {
  cls();
  printf("Dictionary Generator II ver %s\n",ver);
  printf("by Eradicator/Ceara Ratz Org.\n\n");
  if(argc<2) {                                       // Sem parametros
      stdoutx='f';
      printf("Arquivo de saida nao especificado.");
      printf("Gerando em out.dic\n");
      loc++;
      argv[1] = "out.dic" ;      
      }
  if((strcmp(argv[1], "-STDOUT")==0) || (strcmp(argv[1], "-stdout")==0)) stdoutx='t';
  else {
  if((dic=fopen(argv[1],"w"))==NULL) {          // Parametro invalido
	printf(erro2,argv[2]); loc++;
	exit(1);  // Errorlevel 1
	}
      }
  if((config=fopen("dicgen.cfg", "r"))==NULL) {       // Arquivo de Configuracao Invalido
    printf(erro1); loc++;
    fclose(config);
    if((config=fopen("dicgen.cfg", "w"))==NULL) {      // Gera o arquivo de config
      printf(erro4); loc++;
      fclose(config);
      exit(1); // Errorlevel 1
      }
    else {
	  fputs(D_Varis,config);
	  fputs(D_MinMax,config);
	  fputs("---Nao ultrapasse esta linha---\n",config);
	  fputs("1a Linha: Variaveis a serem usadas no wordlist.\n",config);
          fputs("2a Linha: Minimo e Maximo Tamanho da word (01 atÇ 16).\n",config);
	  fclose(config);
	  if((config=fopen("dicgen.cfg", "r"))==NULL) {
	   printf(erro5); loc++;
	   fclose(config);
	   exit(1); // Errorlevel 1
	   }
	 }
    }
  if(fgets(Varis,sizeof(Varis),config)==NULL) {      // 1a Linha do Config
    printf(erro3); loc++;                            // Var: Caracteres
    exit(2); //Errorlevel 2
    }
  if(fgets(Min,sizeof(Min),config)==NULL) {           // 1o Char, 2a Linha
    printf(erro3); loc++;			      // do Config.
    exit(2); //ErrorLevel 2			         Var: Tam. Minimo
    }
  if(fgets(Max,sizeof(Max),config)==NULL) {	      // 2o Char, 2a Linha
    printf(erro3); loc++;			      // do Config.
    exit(2); //ErrorLevel 2                              Var: Tam. Maximo
    }
  fclose(config);                                   // Fecha o file de config

  if(stdoutx=='f') {
    Vezes=0; Bytes=27;
    CheckUp();					    // Mostra tudo q sera feito
    Vezes=0; Bytes=27;

    fprintf(dic,"dICGEn - by ERADiCATor/cr\n");       // Coloca o log na saida
#ifdef TELA
  printf("\nCombinaáoes: %-16s\nNo de Combinaáoes: %010ld em %010ld bytes",Palavra,Vezes,Bytes); loc++;
#endif
    printf("\nPressione Ctrl+C para abortar.\n");
    }
  MakeDic();					    // Combinacoes
  if(stdoutx=='f') {
    fclose(dic);				    // Fecha o file output
    printf("\n%ld combinaáoes em %ld bytes geradas.\n",Vezes,Bytes);
    }
  else {perror("***DONE***"); }
}

/* Sub-Rotina Ajuda do MakeDic()
   Para evitar Out Of Memory da funá∆o principal do MakeDic() */
void MkHlp(int mk)
{
  if(mk==0){
    for(ip=0;ip<=strlen(Varis)-2;ip++)
    {Palavra[14]=Varis[ip];
     Palavra[15]='\0';
     OutPut();
    }
  }

  if(mk==1){
    for(ip=0;ip<=strlen(Varis)-2;ip++)
    {Palavra[14]=Varis[ip];
     for(iq=0;iq<=strlen(Varis)-2;iq++)
      {Palavra[15]=Varis[iq];
       Palavra[16]='\0';
       OutPut();
      }
    }
  }
}


/* =================================================================
   MakeDic() - Ç a parte principal do gerador de wordlist.
      Para os sabe-tudo de programacao que vierem a criticar  essa
      rotina dizendo q ela poderia ser bem melhor bastando colocar
      alguns IFs... Ok.. Faáa um MakeDic() com os teus IFs e vamos
      ver qual Ç a mais r†pida?  Na versao pra pascal,  refiz essa
      sub-rotina  3 vezes pra conseguir uma maior velocidade,  sem
      me importar tanto com o tamanho do c¢digo. Em um programa p/
      gerar wordlist, o q importa nao Ç o tamanho, mas a velocida-
      de.
   ================================================================= */
MakeDic (void)
{                              // Procedure que faz as combinacoes
if(atoi(Min)<=1 && atoi(Max)>=1)                    /* 1 letra */
  {for(ia=0;ia<=strlen(Varis)-2;ia++)
    {Palavra[0]=Varis[ia];
     Palavra[1]='\0';
     OutPut();
    }
  }

if(atoi(Min)<=2 && atoi(Max)>=2)                    /* 2 letras */
  {for(ia=0;ia<=strlen(Varis)-2;ia++)
   {Palavra[0]=Varis[ia];
    for(ib=0;ib<=strlen(Varis)-2;ib++)
    {Palavra[1]=Varis[ib];
     Palavra[2]='\0';
     OutPut();
    }
   }
  }

if(atoi(Min)<=3 && atoi(Max)>=3)                    /* 3 letras */
  {for(ia=0;ia<=strlen(Varis)-2;ia++)
   {Palavra[0]=Varis[ia];
    for(ib=0;ib<=strlen(Varis)-2;ib++)
    {Palavra[1]=Varis[ib];
     for(ic=0;ic<=strlen(Varis)-2;ic++)
     {Palavra[2]=Varis[ic];
      Palavra[3]='\0';
      OutPut();
     }
    }
   }
  }

if(atoi(Min)<=4 && atoi(Max)>=4)                    /* 4 letras */
  {for(ia=0;ia<=strlen(Varis)-2;ia++)
   {Palavra[0]=Varis[ia];
    for(ib=0;ib<=strlen(Varis)-2;ib++)
    {Palavra[1]=Varis[ib];
     for(ic=0;ic<=strlen(Varis)-2;ic++)
     {Palavra[2]=Varis[ic];
      for(id=0;id<=strlen(Varis)-2;id++)
      {Palavra[3]=Varis[id];
       Palavra[4]='\0';
       OutPut();
      }
     }
    }
   }
  }

if(atoi(Min)<=5 && atoi(Max)>=5)                    /* 5 letras */
  {for(ia=0;ia<=strlen(Varis)-2;ia++)
   {Palavra[0]=Varis[ia];
    for(ib=0;ib<=strlen(Varis)-2;ib++)
    {Palavra[1]=Varis[ib];
     for(ic=0;ic<=strlen(Varis)-2;ic++)
     {Palavra[2]=Varis[ic];
      for(id=0;id<=strlen(Varis)-2;id++)
      {Palavra[3]=Varis[id];
       for(ie=0;ie<=strlen(Varis)-2;ie++)
       {Palavra[4]=Varis[ie];
	Palavra[5]='\0';
	OutPut();
       }
      }
     }
    }
   }
  }

if(atoi(Min)<=6 && atoi(Max)>=6)                    /* 6 letras */
  {for(ia=0;ia<=strlen(Varis)-2;ia++)
   {Palavra[0]=Varis[ia];
    for(ib=0;ib<=strlen(Varis)-2;ib++)
    {Palavra[1]=Varis[ib];
     for(ic=0;ic<=strlen(Varis)-2;ic++)
     {Palavra[2]=Varis[ic];
      for(id=0;id<=strlen(Varis)-2;id++)
      {Palavra[3]=Varis[id];
       for(ie=0;ie<=strlen(Varis)-2;ie++)
       {Palavra[4]=Varis[ie];
	for(ig=0;ig<=strlen(Varis)-2;ig++)
	{Palavra[5]=Varis[ig];
	 Palavra[6]='\0';
	 OutPut();
	}
       }
      }
     }
    }
   }
  }

if(atoi(Min)<=7 && atoi(Max)>=7)                    /* 7 letras */
  {for(ia=0;ia<=strlen(Varis)-2;ia++)
   {Palavra[0]=Varis[ia];
    for(ib=0;ib<=strlen(Varis)-2;ib++)
    {Palavra[1]=Varis[ib];
     for(ic=0;ic<=strlen(Varis)-2;ic++)
     {Palavra[2]=Varis[ic];
      for(id=0;id<=strlen(Varis)-2;id++)
      {Palavra[3]=Varis[id];
       for(ie=0;ie<=strlen(Varis)-2;ie++)
       {Palavra[4]=Varis[ie];
	for(ig=0;ig<=strlen(Varis)-2;ig++)
	{Palavra[5]=Varis[ig];
	 for(ih=0;ih<=strlen(Varis)-2;ih++)
	 {Palavra[6]=Varis[ih];
	  Palavra[7]='\0';
	  OutPut();
	 }
	}
       }
      }
     }
    }
   }
  }

if(atoi(Min)<=8 && atoi(Max)>=8)                    /* 8 letras */
  {for(ia=0;ia<=strlen(Varis)-2;ia++)
   {Palavra[0]=Varis[ia];
    for(ib=0;ib<=strlen(Varis)-2;ib++)
    {Palavra[1]=Varis[ib];
     for(ic=0;ic<=strlen(Varis)-2;ic++)
     {Palavra[2]=Varis[ic];
      for(id=0;id<=strlen(Varis)-2;id++)
      {Palavra[3]=Varis[id];
       for(ie=0;ie<=strlen(Varis)-2;ie++)
       {Palavra[4]=Varis[ie];
	for(ig=0;ig<=strlen(Varis)-2;ig++)
	{Palavra[5]=Varis[ig];
	 for(ih=0;ih<=strlen(Varis)-2;ih++)
	 {Palavra[6]=Varis[ih];
	  for(ii=0;ii<=strlen(Varis)-2;ii++)
	  {Palavra[7]=Varis[ii];
	   Palavra[8]='\0';
	   OutPut();
	  }
	 }
	}
       }
      }
     }
    }
   }
  }

if(atoi(Min)<=9 && atoi(Max)>=9)                    /* 9 letras */
  {for(ia=0;ia<=strlen(Varis)-2;ia++)
   {Palavra[0]=Varis[ia];
    for(ib=0;ib<=strlen(Varis)-2;ib++)
    {Palavra[1]=Varis[ib];
     for(ic=0;ic<=strlen(Varis)-2;ic++)
     {Palavra[2]=Varis[ic];
      for(id=0;id<=strlen(Varis)-2;id++)
      {Palavra[3]=Varis[id];
       for(ie=0;ie<=strlen(Varis)-2;ie++)
       {Palavra[4]=Varis[ie];
	for(ig=0;ig<=strlen(Varis)-2;ig++)
	{Palavra[5]=Varis[ig];
	 for(ih=0;ih<=strlen(Varis)-2;ih++)
	 {Palavra[6]=Varis[ih];
	  for(ii=0;ii<=strlen(Varis)-2;ii++)
	  {Palavra[7]=Varis[ii];
	   for(ij=0;ij<=strlen(Varis)-2;ij++)
	   {Palavra[8]=Varis[ij];
	    Palavra[9]='\0';
	    OutPut();
	   }
	  }
	 }
	}
       }
      }
     }
    }
   }
  }

if(atoi(Min)<=10 && atoi(Max)>=10)                    /* 10 letras */
  {for(ia=0;ia<=strlen(Varis)-2;ia++)
   {Palavra[0]=Varis[ia];
    for(ib=0;ib<=strlen(Varis)-2;ib++)
    {Palavra[1]=Varis[ib];
     for(ic=0;ic<=strlen(Varis)-2;ic++)
     {Palavra[2]=Varis[ic];
      for(id=0;id<=strlen(Varis)-2;id++)
      {Palavra[3]=Varis[id];
       for(ie=0;ie<=strlen(Varis)-2;ie++)
       {Palavra[4]=Varis[ie];
	for(ig=0;ig<=strlen(Varis)-2;ig++)
	{Palavra[5]=Varis[ig];
	 for(ih=0;ih<=strlen(Varis)-2;ih++)
	 {Palavra[6]=Varis[ih];
	  for(ii=0;ii<=strlen(Varis)-2;ii++)
	  {Palavra[7]=Varis[ii];
	   for(ij=0;ij<=strlen(Varis)-2;ij++)
	   {Palavra[8]=Varis[ij];
	    for(ik=0;ik<=strlen(Varis)-2;ik++)
	    {Palavra[9]=Varis[ik];
	     Palavra[10]='\0';
	     OutPut();
	    }
	   }
	  }
	 }
	}
       }
      }
     }
    }
   }
  }

if(atoi(Min)<=11 && atoi(Max)>=11)                    /* 11 letras */
  {for(ia=0;ia<=strlen(Varis)-2;ia++)
   {Palavra[0]=Varis[ia];
    for(ib=0;ib<=strlen(Varis)-2;ib++)
    {Palavra[1]=Varis[ib];
     for(ic=0;ic<=strlen(Varis)-2;ic++)
     {Palavra[2]=Varis[ic];
      for(id=0;id<=strlen(Varis)-2;id++)
      {Palavra[3]=Varis[id];
       for(ie=0;ie<=strlen(Varis)-2;ie++)
       {Palavra[4]=Varis[ie];
	for(ig=0;ig<=strlen(Varis)-2;ig++)
	{Palavra[5]=Varis[ig];
	 for(ih=0;ih<=strlen(Varis)-2;ih++)
	 {Palavra[6]=Varis[ih];
	  for(ii=0;ii<=strlen(Varis)-2;ii++)
	  {Palavra[7]=Varis[ii];
	   for(ij=0;ij<=strlen(Varis)-2;ij++)
	   {Palavra[8]=Varis[ij];
	    for(ik=0;ik<=strlen(Varis)-2;ik++)
	    {Palavra[9]=Varis[ik];
	     for(il=0;il<=strlen(Varis)-2;il++)
	     {Palavra[10]=Varis[il];
	      Palavra[11]='\0';
	      OutPut();
	     }
	    }
	   }
	  }
	 }
	}
       }
      }
     }
    }
   }
  }

if(atoi(Min)<=12 && atoi(Max)>=12)                    /* 12 letras */
  {for(ia=0;ia<=strlen(Varis)-2;ia++)
   {Palavra[0]=Varis[ia];
    for(ib=0;ib<=strlen(Varis)-2;ib++)
    {Palavra[1]=Varis[ib];
     for(ic=0;ic<=strlen(Varis)-2;ic++)
     {Palavra[2]=Varis[ic];
      for(id=0;id<=strlen(Varis)-2;id++)
      {Palavra[3]=Varis[id];
       for(ie=0;ie<=strlen(Varis)-2;ie++)
       {Palavra[4]=Varis[ie];
	for(ig=0;ig<=strlen(Varis)-2;ig++)
	{Palavra[5]=Varis[ig];
	 for(ih=0;ih<=strlen(Varis)-2;ih++)
	 {Palavra[6]=Varis[ih];
	  for(ii=0;ii<=strlen(Varis)-2;ii++)
	  {Palavra[7]=Varis[ii];
	   for(ij=0;ij<=strlen(Varis)-2;ij++)
	   {Palavra[8]=Varis[ij];
	    for(ik=0;ik<=strlen(Varis)-2;ik++)
	    {Palavra[9]=Varis[ik];
	     for(il=0;il<=strlen(Varis)-2;il++)
	     {Palavra[10]=Varis[il];
	      for(im=0;im<=strlen(Varis)-2;im++)
	      {Palavra[11]=Varis[im];
	       Palavra[12]='\0';
	       OutPut();
	      }
	     }
	    }
	   }
	  }
	 }
	}
       }
      }
     }
    }
   }
  }

if(atoi(Min)<=13 && atoi(Max)>=13)                    /* 13 letras */
  {for(ia=0;ia<=strlen(Varis)-2;ia++)
   {Palavra[0]=Varis[ia];
    for(ib=0;ib<=strlen(Varis)-2;ib++)
    {Palavra[1]=Varis[ib];
     for(ic=0;ic<=strlen(Varis)-2;ic++)
     {Palavra[2]=Varis[ic];
      for(id=0;id<=strlen(Varis)-2;id++)
      {Palavra[3]=Varis[id];
       for(ie=0;ie<=strlen(Varis)-2;ie++)
       {Palavra[4]=Varis[ie];
	for(ig=0;ig<=strlen(Varis)-2;ig++)
	{Palavra[5]=Varis[ig];
	 for(ih=0;ih<=strlen(Varis)-2;ih++)
	 {Palavra[6]=Varis[ih];
	  for(ii=0;ii<=strlen(Varis)-2;ii++)
	  {Palavra[7]=Varis[ii];
	   for(ij=0;ij<=strlen(Varis)-2;ij++)
	   {Palavra[8]=Varis[ij];
	    for(ik=0;ik<=strlen(Varis)-2;ik++)
	    {Palavra[9]=Varis[ik];
	     for(il=0;il<=strlen(Varis)-2;il++)
	     {Palavra[10]=Varis[il];
	      for(im=0;im<=strlen(Varis)-2;im++)
	      {Palavra[11]=Varis[im];
	       for(in=0;in<=strlen(Varis)-2;in++)
	       {Palavra[12]=Varis[in];
		Palavra[13]='\0';
		OutPut();
	       }
	      }
	     }
	    }
	   }
	  }
	 }
	}
       }
      }
     }
    }
   }
  }

if(atoi(Min)<=14 && atoi(Max)>=14)                    /* 14 letras */
  {for(ia=0;ia<=strlen(Varis)-2;ia++)
   {Palavra[0]=Varis[ia];
    for(ib=0;ib<=strlen(Varis)-2;ib++)
    {Palavra[1]=Varis[ib];
     for(ic=0;ic<=strlen(Varis)-2;ic++)
     {Palavra[2]=Varis[ic];
      for(id=0;id<=strlen(Varis)-2;id++)
      {Palavra[3]=Varis[id];
       for(ie=0;ie<=strlen(Varis)-2;ie++)
       {Palavra[4]=Varis[ie];
	for(ig=0;ig<=strlen(Varis)-2;ig++)
	{Palavra[5]=Varis[ig];
	 for(ih=0;ih<=strlen(Varis)-2;ih++)
	 {Palavra[6]=Varis[ih];
	  for(ii=0;ii<=strlen(Varis)-2;ii++)
	  {Palavra[7]=Varis[ii];
	   for(ij=0;ij<=strlen(Varis)-2;ij++)
	   {Palavra[8]=Varis[ij];
	    for(ik=0;ik<=strlen(Varis)-2;ik++)
	    {Palavra[9]=Varis[ik];
	     for(il=0;il<=strlen(Varis)-2;il++)
	     {Palavra[10]=Varis[il];
	      for(im=0;im<=strlen(Varis)-2;im++)
	      {Palavra[11]=Varis[im];
	       for(in=0;in<=strlen(Varis)-2;in++)
	       {Palavra[12]=Varis[in];
		for(io=0;io<=strlen(Varis)-2;io++)
		{Palavra[13]=Varis[io];
		 Palavra[14]='\0';
		 OutPut();
		}
	       }
	      }
	     }
	    }
	   }
	  }
	 }
	}
       }
      }
     }
    }
   }
  }

if(atoi(Min)<=15 && atoi(Max)>=15)                    /* 15 letras */
  {for(ia=0;ia<=strlen(Varis)-2;ia++)
   {Palavra[0]=Varis[ia];
    for(ib=0;ib<=strlen(Varis)-2;ib++)
    {Palavra[1]=Varis[ib];
     for(ic=0;ic<=strlen(Varis)-2;ic++)
     {Palavra[2]=Varis[ic];
      for(id=0;id<=strlen(Varis)-2;id++)
      {Palavra[3]=Varis[id];
       for(ie=0;ie<=strlen(Varis)-2;ie++)
       {Palavra[4]=Varis[ie];
	for(ig=0;ig<=strlen(Varis)-2;ig++)
	{Palavra[5]=Varis[ig];
	 for(ih=0;ih<=strlen(Varis)-2;ih++)
	 {Palavra[6]=Varis[ih];
	  for(ii=0;ii<=strlen(Varis)-2;ii++)
	  {Palavra[7]=Varis[ii];
	   for(ij=0;ij<=strlen(Varis)-2;ij++)
	   {Palavra[8]=Varis[ij];
	    for(ik=0;ik<=strlen(Varis)-2;ik++)
	    {Palavra[9]=Varis[ik];
	     for(il=0;il<=strlen(Varis)-2;il++)
	     {Palavra[10]=Varis[il];
	      for(im=0;im<=strlen(Varis)-2;im++)
	      {Palavra[11]=Varis[im];
	       for(in=0;in<=strlen(Varis)-2;in++)
	       {Palavra[12]=Varis[in];
		for(io=0;io<=strlen(Varis)-2;io++)
		{Palavra[13]=Varis[io];
		 MkHlp(0);
		}
	       }
	      }
	     }
	    }
	   }
	  }
	 }
	}
       }
      }
     }
    }
   }
  }

if(atoi(Min)<=16 && atoi(Max)>=16)                    /* 16 letras */
  {for(ia=0;ia<=strlen(Varis)-2;ia++)
   {Palavra[0]=Varis[ia];
    for(ib=0;ib<=strlen(Varis)-2;ib++)
    {Palavra[1]=Varis[ib];
     for(ic=0;ic<=strlen(Varis)-2;ic++)
     {Palavra[2]=Varis[ic];
      for(id=0;id<=strlen(Varis)-2;id++)
      {Palavra[3]=Varis[id];
       for(ie=0;ie<=strlen(Varis)-2;ie++)
       {Palavra[4]=Varis[ie];
	for(ig=0;ig<=strlen(Varis)-2;ig++)
	{Palavra[5]=Varis[ig];
	 for(ih=0;ih<=strlen(Varis)-2;ih++)
	 {Palavra[6]=Varis[ih];
	  for(ii=0;ii<=strlen(Varis)-2;ii++)
	  {Palavra[7]=Varis[ii];
	   for(ij=0;ij<=strlen(Varis)-2;ij++)
	   {Palavra[8]=Varis[ij];
	    for(ik=0;ik<=strlen(Varis)-2;ik++)
	    {Palavra[9]=Varis[ik];
	     for(il=0;il<=strlen(Varis)-2;il++)
	     {Palavra[10]=Varis[il];
	      for(im=0;im<=strlen(Varis)-2;im++)
	      {Palavra[11]=Varis[im];
	       for(in=0;in<=strlen(Varis)-2;in++)
	       {Palavra[12]=Varis[in];
		for(io=0;io<=strlen(Varis)-2;io++)
		{Palavra[13]=Varis[io];
		 MkHlp(1);
		}
	       }
	      }
	     }
	    }
	   }
	  }
	 }
	}
       }
      }
     }
    }
   }
  }
return 0;
} //Fim MakeDic

OutPut (void)
{ 					     // Saida
if(stdoutx=='f') {
  fprintf(dic,"%s\n",&Palavra);
  Vezes++;                                  // Adiciona +1 em Vezes
  Bytes=Bytes+strlen(Palavra)+2;            // Adiciona + o num d bytes

#ifdef TELA
  goxy(14,loc); printf("%-16s",Palavra);
  goxy(20,loc+1); printf("%010ld",Vezes);
  goxy(34,loc+1); printf("%010ld",Bytes);
#endif
		}
else {
  printf("%s\n",&Palavra);
     }
return 0;
}



// Funáao potencial
unsigned long pot(long double _num, int _pt)
{int it,x;
 for(it=1,x=1;it<=_pt;it++) {x*= _num;}
 return(x);
}

/* Faz um checkup dizendo quanto de disco ser† utilizado e quantas combinacoes
   ser∆o geradas */
CheckUp(void)
{
 int id_, tamx;
 tamx=strlen(Varis)-1;
 for(id_=1l;id_<=16;id_++)
 {if(atoi(Min)<=id_ && atoi(Max)>=id_)
  {Vezes+=pot(tamx,id_);
   Bytes+=pot(tamx,id_)*(2+id_);
  }
 }
 printf("\nSer∆o geradas %li combinaá‰es em %li bytes.\n",Vezes,Bytes);
 printf("Pressione ENTER para inicial o processo.\n");
 getchar();
 goxy(1,loc+1); printf("                                                                                                                                                               ");
 goxy(1,loc);
 return 0;
}


/* Versao do clrscr() para users de Linux desprovidos do conio.h */
cls(void)
{
 #ifndef linux
  clrscr();
 #else
  printf("[H[J");
 #endif
  return 0;
}

/* Versao do gotoxy() para users de Linux desprovidos do conio.h */
goxy(int y, int x) {
 #ifndef linux
  gotoxy(y,x);
 #else
  printf("[%i;%iH",x,y);
 #endif
 return 0;
}

