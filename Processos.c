#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

pid_t filho1, filho2;

void GestaoProcessos(){
  int opcao=0;
  printf("Consultar PIDs:\n1.PID do processo atual  2.PID do processo pai\nVisualizacao de processos:\n3.ps aux  4.pstree  5.top\nEstatistica Descritiva:\n6.Mediana e Desvio Padrao\n");
  scanf("%d",&opcao);
  switch (opcao) {
    case 1:printf("PID do processo atual:%d\n",getpid());
    break;
    case 2:printf("PID do processo pai:%d\n",getppid());
    break;
    case 3:system("ps aux");
    break;
    case 4:system("pstree");
    break;
    case 5:execl("/usr/bin/top","top",NULL);
    exit(0);
    break;
    case 6: ;
    char nomeFicheiro[100];
    FILE *Data;

    printf("Insira nome do ficheiro a fazer a mediana e o desvio padrao:\n");
    fflush(stdin);
    scanf("%s",nomeFicheiro);

    if(Data = fopen(nomeFicheiro,"rb")){
      printf("O ficheiro inserido foi encontrado!\n");
      printf("Mediana:\n");
      system("awk -F ',' 'NR>1 && NF{count[NR]=$4} END{if(NR%2){print count[(NR+1)/2];}else{print (count[NR/2] + count[(NR/2)+1])/2;}}' PL_Programacao_00_Database.csv");
      printf("Desvio Padrao:\n");
      system("awk -F ',' 'NR > 1 && NF {x[NR]=$4; soma+=$4} END{media=soma/NR; for (i in x){somatorio += (x[i]-media)^2} raiz = sqrt(somatorio/NR); print raiz}' PL_Programacao_00_Database.csv");
      printf("\n");
    }else{
      printf("Nao foi possivel encontrar o ficheiro....\n");
    }
    break;
    default: printf("error\n");
  }
}

void CriacaoProcessos(){
  int opcao=0;
  printf("1.Calculo da mediana, maximo e minimo  2.Apagar ficheiros de resultados  0.Sair\n");
  scanf("%d",&opcao);
  switch (opcao) {
    case 1: ;
    char nomeFicheiro[100];
    char linha[100];
    char coluna[100];
    FILE *Data;

    printf("Insira nome do ficheiro a fazer a mediana, o maximo e o minimo:\n");
    fflush(stdin);
    scanf("%s",nomeFicheiro);

    printf("Insira a linha cabecalho do ficheiro:\n");
    fflush(stdin);
    scanf("%s",linha);

    printf("Insira a coluna de dados a analisar:\n");
    fflush(stdin);
    scanf("%s",coluna);

    filho1 = fork();

    if(Data = fopen(nomeFicheiro,"rb")){
      if(filho1 < 0){
        printf("Erro - filho1\n");
      }else{
        if(filho1 == 0){
          //calculaMediana
          char calculaMediana[100] = "awk -F ',' 'NR>1 && NF{count[NR]=$";

          strcat(coluna, "} END{if(NR%2){print count[(NR+1)/2];}else{print (count[NR/2] + count[(NR/2)+1])/2;}}' ");
          strcat(coluna,nomeFicheiro);
          strcat(coluna, " >> ficha02_mediana.dat");
          strcat(calculaMediana,coluna);

          system(calculaMediana);

          //calculaMaximo
          char calculaMaximo[100] = ("awk -F ',' 'BEGIN{numeroMaximo=0}{if ($");

          strcat(coluna,">0+numeroMaximo) numeroMaximo=$");
          strcat(coluna,"} END{print numeroMaximo}' ");
          strcat(coluna,nomeFicheiro);
          strcat(coluna," >> ficha02_mediana.dat");
          strcat(calculaMaximo,coluna);

          system(calculaMaximo);

          //calculaMinimo
          char calculaMinimo[100] = ("awk -F ',' 'BEGIN{numeroMinimo=500}{if ($");

          strcat(coluna,"<0+numeroMinimo) numeroMinimo=$");
          strcat(coluna,"} END{print numeroMinimo}' ");
          strcat(coluna,nomeFicheiro);
          strcat(coluna," >> ficha02_mediana.dat");
          strcat(calculaMinimo,coluna);

          system(calculaMinimo);

          //escreveFicheiroSucesso
          FILE * ficheiroSucesso = fopen("ficha02_sucesso.dat","a+b");
          time_t tempoAtual = time(&tempoAtual);
          struct tm *ponteiroTempo = gmtime(&tempoAtual);

          char informacaoSucesso[] = ("Operacao realizada com sucesso. Data:");
          strcat(informacaoSucesso,asctime(ponteiroTempo));

          fwrite(informacaoSucesso,sizeof(informacaoSucesso),1,ficheiroSucesso);
          fclose(ficheiroSucesso);

          printf("A informacao foi enviada para o ficheiro ficha02_mediana.dat com sucesso. PID:%d\n",getpid());
        }
      }

      filho2 = fork();

      if(filho2<0){
          printf("Erro - filho2\n");
      }else{
        if(filho2 == 0){
          printf("Conteudo do ficheiro ficha02_mediana.dat:\n");
          system("cat ficha02_mediana.dat");
          printf("Conteudo do ficheiro ficha02_sucesso.dat:\n");
          system("cat ficha02_sucesso.dat");

          //Função Grafico
          system("gnuplot -e \"plot 'ficha02_mediana.dat' title 'mediana';pause mouse;\"");

          printf("A informacao do grafico foi mostrado com sucesso. PID:%d\n",getpid());
          exit(0);
        }
      }
    }
    break;
    case 2: ;
    int estado;

    filho1 = fork();
    filho2 = fork();

    if(filho1 < 0 && filho2 < 0){
      printf("Erro - filhos\n");
    }else{
      if(filho1 == 0 && filho2 == 0){
        system("rm ficha02_mediana.dat ficha02_sucesso.dat");

        printf("Os ficheiros foram apagados com sucesso. PID:%d\n",getpid());
      }else{
        filho1 = wait(&estado);
        filho2 = wait(&estado);

        printf("Os processos filho terminaram!. PID:%d\n",getpid());
      }
    }
    break;
    default: printf("Erro\n");
  }
}
void calculos()
{
  char nomeFicheiro[100];
  char linha[100];
  char coluna[100];
  FILE *Data;
  signal(SIGUSR1, calculos);
  printf("Sinal SIGUSR1 recebido pelo processo com PID %d\n", getpid());
  printf("Insira nome do ficheiro a fazer a mediana, o maximo e o minimo:\n");
  fflush(stdin);
  scanf("%s",nomeFicheiro);

  printf("Insira a linha cabecalho do ficheiro:\n");
  fflush(stdin);
  scanf("%s",linha);

  printf("Insira a coluna de dados a analisar:\n");
  fflush(stdin);
  scanf("%s",coluna);
  if(Data = fopen(nomeFicheiro,"rb")){
    char calculaMediana[100] = "awk -F ',' 'NR>1 && NF{count[NR]=$";

    strcat(coluna, "} END{if(NR%2){print count[(NR+1)/2];}else{print (count[NR/2] + count[(NR/2)+1])/2;}}' ");
    strcat(coluna,nomeFicheiro);
    strcat(coluna, " >> ficha03_mediana.dat");
    strcat(calculaMediana,coluna);

    system(calculaMediana);

    //calculaMaximo
    char calculaMaximo[100] = ("awk -F ',' 'BEGIN{numeroMaximo=0}{if ($");

    strcat(coluna,">0+numeroMaximo) numeroMaximo=$");
    strcat(coluna,"} END{print numeroMaximo}' ");
    strcat(coluna,nomeFicheiro);
    strcat(coluna," >> ficha03_mediana.dat");
    strcat(calculaMaximo,coluna);

    system(calculaMaximo);

    //calculaMinimo
    char calculaMinimo[100] = ("awk -F ',' 'BEGIN{numeroMinimo=500}{if ($");

    strcat(coluna,"<0+numeroMinimo) numeroMinimo=$");
    strcat(coluna,"} END{print numeroMinimo}' ");
    strcat(coluna,nomeFicheiro);
    strcat(coluna," >> ficha03_mediana.dat");
    strcat(calculaMinimo,coluna);

    system(calculaMinimo);

    //escreveFicheiroSucesso
    FILE * ficheiroSucesso = fopen("ficha03_sucesso.dat","a+b");
    time_t tempoAtual = time(&tempoAtual);
    struct tm *ponteiroTempo = gmtime(&tempoAtual);

    char informacaoSucesso[] = ("Operacao realizada com sucesso. Data:");
    strcat(informacaoSucesso,asctime(ponteiroTempo));

    fwrite(informacaoSucesso,sizeof(informacaoSucesso),1,ficheiroSucesso);
    fclose(ficheiroSucesso);

    printf("A informacao foi enviada para o ficheiro ficha03_mediana.dat com sucesso. PID:%d\n",getpid());
  }
}
void mostrar(){
  signal(SIGUSR2, mostrar);
  printf("Sinal SIGUSR2 recebido pelo processo com PID %d\n", getpid());
  printf("Conteudo do ficheiro ficha03_mediana.dat:\n");
  system("cat ficha03_mediana.dat");
  printf("Conteudo do ficheiro ficha03_sucesso.dat:\n");
  system("cat ficha03_sucesso.dat");

  //Função Grafico
  system("gnuplot -e \"plot 'ficha03_mediana.dat' title 'mediana';pause mouse;\"");

  printf("A informacao do grafico foi mostrado com sucesso. PID:%d\n",getpid());
  exit(0);
}
void apagar()
{
  signal(SIGTERM, apagar);
  printf("Sinal SIGTERM recebido pelo processo com PID %d\n", getpid());
  system("rm ficha03_mediana.dat ficha03_sucesso.dat");

  printf("Os ficheiros foram apagados com sucesso. PID:%d\n",getpid());
  exit(0);
}
void Signals(){
  int opcao=0;
  printf("1.Calculo da mediana, maximo e minimo  2.Apagar ficheiros de resultados  0.Sair\n");
  scanf("%d",&opcao);
  switch (opcao) {
    case 1: ;
    pid_t filhoa, filhob;
    filhoa = fork();
    if(filhoa < 0){
      printf("Erro - filhoA\n");
    }else{
      if(filhoa == 0){
        signal(SIGUSR1, calculos);
        signal(SIGUSR2, mostrar);
        for(;;);

      }else{
        filhob = fork();
        if(filhob < 0){
          printf("Erro - filhoB\n");
        }else{
          if(filhob == 0)
          {

          }else{
            printf("Sou o processo pai (PID %d),e criei os processosA (PID %d) e B(PID %d)\n", getppid(), filhoa, filhob);
            printf("Sinal SIGUSR1 enviado pelo processo pai(PID %d)ao processo filhoA (PID %d)", getppid(), filhoa);
            kill(filhoa, SIGUSR1);
            sleep(100);
            printf("Sinal SIGUSR2 enviado pelo processo pai(PID %d)ao processo filhoA (PID %d)", getppid(), filhoa);
            kill(filhoa, SIGUSR2);
            sleep(3);
          }
        }
    }
  }
  break;
  case 2: ;
  filhoa = fork();
  if(filhoa < 0){
    printf("Erro - filhoA\n");
  }else{
    if(filhoa == 0){
      signal(SIGUSR1, calculos);
      signal(SIGUSR2, mostrar);
      for(;;);

    }else{
      filhob = fork();
      if(filhob < 0){
        printf("Erro - filhoB\n");
      }else{
        if(filhob == 0)
        {
          signal(SIGTERM, apagar);
          for(;;);
        }else{
          printf("Sou o processo pai (PID %d),e criei os processosA (PID %d) e B(PID %d)\n", getppid(), filhoa, filhob);
          printf("Sinal SIGTERM enviado pelo processo pai(PID %d)ao processo filhoB (PID %d)", getppid(), filhob);
          kill(filhob, SIGTERM);
          sleep(3);
        }
      }
  }
}break;

}


}

void main(){
  int menu=0;
  printf("Bem vindo!\n");
  do{
  printf("1.Gestao de processos  2.Criacao de processos  3.Signals 0.Sair\n");
  scanf("%d",&menu);
  switch (menu) {
    case 0:printf("Adeus!\n");
    return;
    break;
    case 1:GestaoProcessos();
    break;
    case 2:CriacaoProcessos();
    break;
    case 3:Signals();
    break;
    default: printf("error\n");
  }
  }while(1);
}
