#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>

void gerarVetor(int n){
    srand(time(NULL));
    int vetorAleatorio[n];
    for (int i=0;i<n;i++){
        vetorAleatorio[i] = (rand()%100)+1;
    }
}
void* funcaoSoma(){
  int acumulador;
  for(int i=0; i<100;i++){
    acumulador++;
  }
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