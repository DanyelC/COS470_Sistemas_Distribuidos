#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>

#define quantityThreads 2
#define numberN 10000000
// realizar a soma dos elementos de um vetor de n termos utilizando k threads
//limitar acesso a variável global acumulador atraves de acquire/ release/ test and set
int acumulador = 0;
int vetorAleatorio[numberN];

struct lock {
    int held;
};
void acquire(struct lock cadeado) {
    while (__sync_lock_test_and_set(&cadeado.held, 1))
; //busy wait
}

void release(struct lock *cadeado) {
    cadeado->held = 0;
}

typedef struct argDado {
    int numeroThread;
} argDado;
void gerarVetor(int n){
    srand(time(NULL));
    int vetorAleatorio[n];
   for (int i=0;i<n;i++){
        vetorAleatorio[i] = (rand()%200)-99;
    }
  }
void* funcaoSoma(void* arg){
  argDado* dadoThread = (argDado*)arg;
  int fim = (dadoThread->numeroThread) * (numberN/quantityThreads);
  int inicio = fim - (numberN/quantityThreads);
  int somaParcial = 0;
  for(int i=inicio; i<fim;i++){
    somaParcial += vetorAleatorio[i];
  }
  acquire();
  acumulador += somaParcial;
  release();

  return NULL;
}


void gerandoThreads(int k){
  pthread_t th[k];
  int i;
  //serializar threads aqui
  for(i=0;i<k;i++){
    pthread_create(th + i, NULL, &funcaoSoma, NULL);
  }
  for(i=0;i<k;i++){
    pthread_join(th[i], NULL);
  }
}


