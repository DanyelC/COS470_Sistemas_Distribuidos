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
struct estruturaThreads {
    int numeroThread;
    struct lock cadeado;
};
void acquire(struct lock cadeado) {
    while (__sync_lock_test_and_set(&cadeado.held, 1))
; //busy wait
}

void release(struct lock *cadeado) {
    cadeado->held = 0;
}

void gerarVetor(){
   srand(time(NULL));
   int vetorAleatorio[numberN];
   for (int i=0;i<numberN;i++){
        vetorAleatorio[i] = (rand()%201)-100; 
    }
  }
void* funcaoSoma(void* arg){ //asteriscos

  struct estruturaThreads *args = arg;
  int somaParcial = 0;
  int fim = (args->numeroThread) * (numberN/quantityThreads);
  int inicio = fim - (numberN/quantityThreads);
  for(int i=inicio; i<fim;i++){
    somaParcial += vetorAleatorio[i];
  }
   acquire(args->cadeado);
   acumulador += somaParcial;
   release(&args->cadeado);
  
  return NULL;
}


void gerandoThreads(){
  // pthread_t *th = (pthread_t *)malloc(sizeof(pthread_t) * K);
  struct lock cadeado;
  cadeado.held = 0;  
  pthread_t th[quantityThreads];
  struct estruturaThreads args[quantityThreads];
  int i;
  for(i=0;i<quantityThreads;i++){
    args[i].numeroThread = i;
    args[i].cadeado = cadeado; 
    pthread_create(th + i, NULL, &funcaoSoma, (void *)&args[i]);
  }
  for(i=0;i<quantityThreads;i++){
    pthread_join(th[i], NULL);
  }
}
int main(){
  clock_t startTime, endTime;
  double deltaT = 0;
  for(int i = 0; i<10;i++){ //media do deltaT
    gerarVetor();
    startTime = clock();
    gerandoThreads(); //threads atuam na fç soma
    endTime = clock();
    deltaT += (double)(endTime - startTime) / CLOCKS_PER_SEC;
  }
  printf("Tempo médio: %f",deltaT/10);
  
  return 0;
}


