#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <sys/types.h>

pid_t filho1, filho2;
char ficheiro[500];

//ficha4
//struct ficha4{
//  int c1;
//  int c2;
//  int repeticoes;
//}
int repeticoes;
pthread_mutex_t mutex;
sem_t sem;
struct mensagem{
    long tipo;
    char texto[100];
}mensagem;


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

void *TransformaLinhasColunas(){
  char auxFicheiro[500];
  char novoFicheiro[500];
  char auxToken[500][500];
  char comando[500];
  int i=0;
  printf("Insira o nome do ficheiro:");
  fflush(stdin);
  scanf("%s",&ficheiro);

  strcat(auxFicheiro, ficheiro);
  char * token = strtok(ficheiro, ".");

  do{
    strcat(auxToken[i], token);
    token = strtok(NULL, ".");
    i++;
  }while(token !=NULL);

  strcat(novoFicheiro, auxToken[i-2]);
  strcat(novoFicheiro, "_inv.");
  strcat(novoFicheiro, auxToken[i-1]);

  strcat(comando, "awk -F, '{for(i=1; i<=NF; i++){A[NR, i]=$i}; if(NF>n){n=NF}} END{for(i=0; i<n; i++){for(j=0;j<NR;j++){s=s?s\", \"A[j, i]:A[j, i]} print s;s=\"\"}}' ");
  strcat(comando, auxFicheiro);
  strcat(comando, " > ");
  strcat(comando, novoFicheiro);
  system(comando);

  printf("Geracao do ficheiro com troca de colunas com linhas efetuada com sucesso. Insira qualquer tecla para continuar... \n");
  char a;
  scanf("%s",&a);
  pthread_exit(0);
}

void *ficha4thread2(void *coluna1){
  char comando[500];
  char buffer1[500];
  char buffer2[500];

  int c1 = *(int*)coluna1;

  strcat(comando, "awk -F ',' '{linha[NR]=$");
  sprintf(buffer1, "%d", c1);
  strcat(comando, buffer1);
  strcat(comando, "} END{for(x=0; x<");
  sprintf(buffer2, "%d", repeticoes);
  strcat(comando, buffer2);
  strcat(comando, "; x++){for(i=0; i<NR; i++) {printf \"%s \", linha[i]}{printf \"\\n\"}} }' ");
  strcat(comando, ficheiro);
  strcat(comando, " >> PL_Programacao_04_Threads.csv");
  system(comando);
  printf("\nThread2 %d do processo %d escreveu com sucesso no ficheiro PL_Programacao_04_Threads.csv, a coluna %d repetida %d vezes! Insira qualquer tecla para continuar...", pthread_self(), getppid(), c1, repeticoes);
  char a;
  scanf("%s",&a);
  pthread_exit(NULL);
}

void *ficha4thread3(void *coluna2){
  char comando[500];
  char buffer1[500];
  char buffer2[500];

  int c2 = *(int*)coluna2;

  strcat(comando, "awk -F ',' '{linha2[NR]=$");
  sprintf(buffer1, "%d", c2);
  strcat(comando, buffer1);
  strcat(comando, "} END{for(x=0; x<");
  sprintf(buffer2, "%d", repeticoes);
  strcat(comando, buffer2);
  strcat(comando, "; x++){for(i=0; i<NR; i++) {printf \"%s \", linha2[i]}{printf \"\\n\"}} }' ");
  strcat(comando, "ficheiro");
  strcat(comando, " >> PL_Programacao_04_Threads.csv");
  system(comando);
  printf("\nThread3 %d do processo %d escreveu com sucesso no ficheiro PL_Programacao_04_Threads.csv, a coluna %d repetida %d vezes! Insira qualquer tecla para continuar...", pthread_self(), getppid(), c2, repeticoes);
  char a;
  scanf("%s",&a);
  pthread_exit(NULL);
}

void Threads(){
  int opcao;
  pthread_t thread1,thread2,thread3;
  int c1=0, c2=0;

  printf("1.Transformacao de colunas em linhas  2.Extracao e gravacao de linhas\n");
  scanf("%d",&opcao);
  switch (opcao) {
    case 1:
    pthread_create(&thread1, NULL, TransformaLinhasColunas, NULL);
    pthread_join(thread1, NULL);
    break;

    case 2:
    system(" > PL_Programacao_04_Threads.csv ");
    printf("\nGeração do ficheiro PL_Programacao_04_Threads.csv efetuada com sucesso!");

    printf("\nInsira o nome do ficheiro: ");
    scanf("%s", &ficheiro);
    printf("\nInsira a primeira coluna: ");
    scanf("%d", &c1);
    printf("\nInsira a segunda coluna: ");
    scanf("%d", &c2);
    printf("\nInsira o número de repetições: ");
    scanf("%d", &repeticoes);

    pthread_create(&thread2, NULL, ficha4thread2, &c1);
    pthread_join(thread2, NULL);

    pthread_create(&thread3, NULL, ficha4thread3, &c2);
    pthread_join(thread3, NULL);
    break;
  }
}

void *ficha5mutex(void *coluna1){
  char comando[500];
  char buffer1[500];
  char buffer2[500];

  int c1 = *(int*)coluna1;
  pthread_mutex_lock(&mutex);
   printf("%d", c1);
  strcat(comando, "awk -F ',' '{linha[NR]=$");
  sprintf(buffer1, "%d", c1);
  strcat(comando, buffer1);
  strcat(comando, "} END{for(x=0; x<");
  sprintf(buffer2, "%d", repeticoes);
  strcat(comando, buffer2);
  strcat(comando, "; x++){for(i=0; i<NR; i++) {printf \"%s \", linha[i]}{printf \"\\n\"}} }' ");
  strcat(comando, ficheiro);
  strcat(comando, " >> PL_Programacao_05_Threads.csv");
  system(comando);
  pthread_mutex_unlock(&mutex);
  printf("\nThread %d do processo %d escreveu com sucesso no ficheiro PL_Programacao_05_Threads.csv, a coluna %d repetida %d vezes! Insira qualquer tecla para continuar...", pthread_self(), getppid(), c1, repeticoes);
  char a;
  scanf("%s",&a);
  pthread_exit(NULL);
}

void *ficha5sem(void *coluna1){
  char comando[500];
  char buffer1[500];
  char buffer2[500];

  int c1 = *(int*)coluna1;

  sem_wait(&sem);

   printf("%d", c1);
  strcat(comando, "awk -F ',' '{linha[NR]=$");
  sprintf(buffer1, "%d", c1);
  strcat(comando, buffer1);
  strcat(comando, "} END{for(x=0; x<");
  sprintf(buffer2, "%d", repeticoes);
  strcat(comando, buffer2);
  strcat(comando, "; x++){for(i=0; i<NR; i++) {printf \"%s \", linha[i]}{printf \"\\n\"}} }' ");
  strcat(comando, ficheiro);
  strcat(comando, " >> PL_Programacao_05_Threads.csv");
  system(comando);

  sem_post(&sem);

  printf("\nThread %d do processo %d escreveu com sucesso no ficheiro PL_Programacao_05_Threads.csv, a coluna %d repetida %d vezes! Insira qualquer tecla para continuar...", pthread_self(), getppid(), c1, repeticoes);
  char a;
  scanf("%s",&a);
  pthread_exit(NULL);
}

void Sincronizacao(){
int opcao;
  pthread_t thread1,thread2,thread3, thread4;
  int c1=0, c2=0;

  printf("1.Extracao e gravacao de colunas(pthread_mutex)  2.Extracao e gravacao de colunas(sem)\n");
  scanf("%d",&opcao);
  switch (opcao) {
    case 1:
    system(" > PL_Programacao_05_Threads.csv ");
    printf("\nGeração do ficheiro PL_Programacao_05_Threads.csv efetuada com sucesso!");

    printf("\nInsira o nome do ficheiro: ");
    scanf("%s", &ficheiro);
    printf("\nInsira a primeira coluna: ");
    scanf("%d", &c1);
    printf("\nInsira a segunda coluna: ");
    scanf("%d", &c2);
    printf("\nInsira o número de repetições: ");
    scanf("%d", &repeticoes);

    pthread_mutex_init(&mutex,NULL);

    pthread_create(&thread1, NULL, ficha5mutex, &c1);
    pthread_create(&thread2, NULL, ficha5mutex, &c2);

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    pthread_mutex_destroy(&mutex);
    break;

    case 2:
    system(" > PL_Programacao_05_Threads.csv ");
    printf("\nGeração do ficheiro PL_Programacao_05_Threads.csv efetuada com sucesso!");

    printf("\nInsira o nome do ficheiro: ");
    scanf("%s", &ficheiro);
    printf("\nInsira a primeira coluna: ");
    scanf("%d", &c1);
    printf("\nInsira a segunda coluna: ");
    scanf("%d", &c2);
    printf("\nInsira o número de repetições: ");
    scanf("%d", &repeticoes);

    sem_init(&sem, 0, 1);

    pthread_create(&thread3, NULL, ficha5sem, &c1);
    pthread_create(&thread4, NULL, ficha5sem, &c2);

    pthread_join(thread3, NULL);
    pthread_join(thread4, NULL);

    sem_destroy(&sem);
    break;
  }
}
void ficha6(){
  int msgid;
    msgid = msgget((key_t) 1234, 0666 | IPC_CREAT);
    if(fork() == 0)
    {
      FILE *fp;
      fp = fopen("PL_Programacao_06_IPC.dat", "wb");
      do {
        msgrcv(msgid, &mensagem, sizeof(mensagem.texto), 1, 0);
        fprintf(fp, "%s\n", mensagem.texto);
      } while(strcmp(mensagem.texto, "terminar") && strcmp(mensagem.texto, "Terminar"));

      msgctl(msgid, IPC_RMID, 0);
      fclose(fp);
      char conteudo[100];
      fp = fopen("PL_Programacao_06_IPC.dat", "rb");
      printf("Mensagens recebidas:\n");
      while (fscanf(fp, "%s\n", conteudo)!= EOF) {
        printf("%s\n", conteudo);
      }
      exit(0);
    }else
    {
      mensagem.tipo = 1;
      do {
        printf("Write Data : ");
        scanf("%s\n", &mensagem.texto);
        msgsnd(msgid, &mensagem, sizeof(mensagem.texto), 0);
      } while(strcmp(mensagem.texto, "terminar") && strcmp(mensagem.texto, "Terminar"));

      wait(NULL);
    }

}

void flush_in()
{
   int ch;

   while( (ch = fgetc(stdin)) != EOF && ch != '\n' ){}
}

void IPC(){
  int opcao;
  do{
    printf("1.Envio de mensagens entre processos  2. Sair\n");

    flush_in();

    scanf("%d", &opcao);
    switch (opcao) {
      case 1: ficha6();
      break;
      case 2: printf("A voltar ao menu!!\n");
      break;
      default: printf("Opcao nao reconhecida!!!\n");
      break;
    }
  } while(opcao != 2);
}

void main(){
  int menu=0;
  printf("Bem vindo!\n");
  do{
  printf("1.Gestao de processos  2.Criacao de processos  3.Signals  4.Threads  5.Sincronizacao  6.IPC  0.Sair\n");
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
    case 4:Threads();
    break;
    case 5:Sincronizacao();
    break;
    case 6:IPC();
    break;
    default: printf("error\n");
  }
  }while(1);
}
