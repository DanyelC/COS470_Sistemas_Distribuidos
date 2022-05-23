#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>

// realizar a soma dos elementos de um vetor de n termos utilizando k threads
//limitar acesso a variável global acumulador atraves de acquire/ release/ test and set
int acumulador;
void gerarVetor(int n){
    srand(time(NULL));
    int vetorAleatorio[n];
    for (int i=0;i<n;i++){
        vetorAleatorio[i] = (rand()%200)-99;
    }
}
void* funcaoSoma(){
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