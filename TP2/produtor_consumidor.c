#include <math.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

// rodar com gcc teste.c -o teste -lpthread -lrt -lm

/*
  problemas :

    acesso a memoria compartilhada --> semaforo mutex
    o que fazer quando a memória esta cheia/vazia? Checar se esta vazio ou cheio --> semaforo contador

*/

/*
  definir n = tamanho do N_pilha =============================================== OK

  definir nc e np = numero de threads de cada tipo =============================== OK

  produtor gera de 1 - 10^7 e bota na mem  ======================================== OK

  consumidor pega o numero, esvazia o local e verifica se é primo. imprime no
  terminal o resultado ============================================================= OK

  ordem na mem = semaforos =========================================================== OK

  se pode entrar no N_pilha ou nao = semaforo contador ================================= OK

*/

sem_t semEmpty;
sem_t semFull;

pthread_mutex_t mutexN_pilha; //semaforo mutex p acesso a pilha

int *N_pilha= NULL; // será usado como uma N_pilha
int contador = 0; // usado para marcar em qual indice deve ser colocado/lido o número
int ja_processados = 0; // para limitar o numero de valores consumidos

void is_prime(int ni) {
  for (int i = 2; i <= sqrt(ni); i++) {
    if (ni % i == 0) {
      printf("O %d não é primo.\n", ni);
      return;
    }
  }
  if (ni <= 1) {
    printf("O %d não é primo.\n", ni);
    return;
  }
  printf("O %d é primo. WOW!\n", ni);
}

// Produtor não se preocupa com quantos já foram consumidos. Limitaçao e no consumidor
void *produzir(void *args) {
  srand(time(NULL));
  while (true) {
    
    int r_numb = rand() % (int)pow(10, 7) + 1;
    
    sem_wait(&semEmpty);
    pthread_mutex_lock(&mutexN_pilha);
    N_pilha[contador] = r_numb;
    contador++;
    pthread_mutex_unlock(&mutexN_pilha);
    sem_post(&semFull);
  }
}

//precisa verificar quantos numeros ja foram processados
void *consumir(void *args) {
  while (ja_processados < 0){// (int)pow(10,5)) {
    sem_wait(&semFull);
    pthread_mutex_lock(&mutexN_pilha);
    int ni = N_pilha[contador - 1];
    is_prime(ni); // talvez tirar do semaforo
    contador--;
    ja_processados++;
    pthread_mutex_unlock(&mutexN_pilha);
    sem_post(&semEmpty);
  }
  exit(0); // Quando processar 10^5, mata o processo. Roubado dms?
}


int main(int argc, char *argv[]) {
  char *p; // pega valores do terminal
  int N, threads, nc, np;
  
  N = strtol(argv[1], &p, 10);
  np = strtol(argv[2], &p, 10);
  nc = strtol(argv[3], &p, 10);
  N_pilha = malloc(N * sizeof(int)); // alocaçao dinamica do tamanho da pilha para facilitar
  threads = nc + np;
  
  //Checagem basica
  if(nc <= 0 || np <= 0 || N<=0){
    printf("Tamanho da pilha: %d\nNúmero de Threads Totais: %d\nNúmero de Threads Produtoras: %d\nNúmero de Threads Consumidoras: %d\n", N, threads, np, nc);
    printf("Faz sentido isso? Tem que ter no mínimo 1 Thread Produtora e Consumidora. Ah, o tamanho da pilha tem que ser maior que 0\n");
    return 0;
  }
  
  printf("Tamanho da pilha: %d\nNúmero de Threads Totais: %d\nNúmero de Threads Produtoras: %d\nNúmero de Threads Consumidoras: %d\n", N, threads, np, nc);

  pthread_t th[threads]; // criando as threads
  pthread_mutex_init(&mutexN_pilha, NULL);
  sem_init(&semEmpty, 0, 10);
  sem_init(&semFull, 0, 0);
  for (int i = 0; i < threads; i++) {
    // criando np produtoras. o resto é consumidora
    if (i < np) { 
      pthread_create(&th[i], NULL, &produzir, NULL);
    } else {
      pthread_create(&th[i], NULL, &consumir, NULL);
    }
  }
  for (int i = 0; i < threads; i++) {
    pthread_join(th[i], NULL);
  }
  sem_destroy(&semEmpty);
  sem_destroy(&semFull);
  pthread_mutex_destroy(&mutexN_pilha);
  return 0;
}