#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define quantityThreads 2
#define numberN 10000000
// realizar a soma dos elementos de um vetor de n termos utilizando k threads
// limitar acesso a variável global acumulador atraves de acquire/ release/ test
// and set


int acumulador;
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
    ; // busy wait
}

void release(struct lock *cadeado) { cadeado->held = 0; } 
//libera acesso para outra thread
void gerarVetor() {
  srand(time(NULL));
  for (int i = 0; i < numberN; i++) {
    vetorAleatorio[i] = (rand() % 201) - 100; //intervalo:-100 a 100
  }
}

void *funcaoSoma(void *arg) { 

  struct estruturaThreads *args = arg;
  int somaParcial = 0;
  int fim = (args->numeroThread) *
            (int)(numberN / quantityThreads); 
  int inicio =
      fim - (int)(numberN / quantityThreads); 
  // delimita região do vetor acessada pela thread
  for (int i = inicio; i < fim; i++) {
    somaParcial += vetorAleatorio[i];
  }
  acquire(args->cadeado);
  acumulador += somaParcial; //serializando acesso a variavel global
  release(&args->cadeado);

  return NULL;
}

void gerandoThreads() {
  pthread_t th[quantityThreads];
  struct lock cadeado;
  cadeado.held = 0;
  struct estruturaThreads args[quantityThreads];
  for (int i = 0; i < quantityThreads; i++) {
    args[i].numeroThread = i + 1; 
    args[i].cadeado = cadeado;
    pthread_create(&th[i], NULL, &funcaoSoma,
                   (void *)&args[i]); 
  }
  for (int i = 0; i < quantityThreads; i++) {
    pthread_join(th[i], NULL);
  }
}

int main() {
  clock_t startTime, endTime;
  double deltaT = 0;
  for (int i = 0; i < 10; i++) { // media do deltaT
    acumulador = 0;
    gerarVetor();
    startTime = clock();
    gerandoThreads(); // threads atuam na fç soma
    endTime = clock();
    deltaT += (double)(endTime - startTime) / CLOCKS_PER_SEC;
    printf("Acumulador: %d\n", acumulador);
  }
  printf("Tempo médio: %f\n", deltaT / 10);
  printf("Acumulador: %d\n", acumulador);
  return 0;
}
