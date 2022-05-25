#include <math.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

pthread_mutex_t mutexN_pilha; // semaforo mutex p adicionar/apagar da pilha
sem_t semaforo_livre;         // semaforo contador de posições livres
sem_t semaforo_preenchido;    // semaforo contador de posições ocupadas

int *N_pilha = NULL; // será usado como uma pilha --> facilita o processo
pthread_t *th = NULL;
int threads;
int contador =
    0; // usado para marcar em qual indice deve ser colocado/lido o número
int ja_processados = 0; // para limitar o numero de valores consumidos

int printar = 0;

void is_prime(int ni) {
  for (int i = 2; i <= sqrt(ni); i++) {
    if (ni % i == 0) {
      if (printar != 0) {
        printf("O %d não é primo.\n", ni);
      }
      return;
    }
  }
  if (ni <= 1) {
    if (printar != 0) {
      printf("O %d não é primo.\n", ni);
    }
    return;
  }
  if (printar != 0) {
    printf("O %d é primo. WOW!\n", ni);
  }
}

// precisa verificar quantos numeros ja foram processados, senao vai rodar
// infinitamente.
void *produzir() {
  srand(time(NULL));
  while (ja_processados < 100000) {
    int r_numb = rand() % (int)pow(10, 7) + 1;
    if (printar != 0) {
      printf("Produzido: %d\n", r_numb);
    }
    sem_wait(
        &semaforo_livre); // só produz se tiver espaço livre. Fica esperando
    pthread_mutex_lock(&mutexN_pilha);
    N_pilha[contador] = r_numb;
    contador++;
    pthread_mutex_unlock(&mutexN_pilha);
    sem_post(&semaforo_preenchido); // avisa que produziu 1
  }
  printf("FIM PRODUÇÃO! %d Produzidos\n", ja_processados);
  return NULL;
}

void mata_tudo() {
  sem_destroy(&semaforo_livre);
  sem_destroy(&semaforo_preenchido);
  pthread_mutex_destroy(&mutexN_pilha);
  for (int i = 0; i < threads; i++) {
    pthread_cancel(th[i]);
  }
}

// precisa verificar quantos numeros ja foram processados
void *consumir() {
  int ni = N_pilha[contador - 1];
  while (ja_processados < 100000) {
    sem_wait(&semaforo_preenchido); // só produz se tiver espaço preenchido.
                                    // Fica esperando
    is_prime(ni);
    pthread_mutex_lock(&mutexN_pilha);
    ja_processados++;
    contador--;
    pthread_mutex_unlock(&mutexN_pilha);
    sem_post(&semaforo_livre); // avisa que consumiu
  }
  printf("FIM CONSUMO! Consumidos %d\n", ja_processados);
  mata_tudo();
  return NULL;
}

int main(int argc, char *argv[]) {
  char *p; // pega valores do terminal
  int N, nc, np;
  clock_t start, finish;
  double media = 0;
  FILE *file = fopen("resultados.csv", "a");

  N = strtol(argv[1], &p, 10);
  np = strtol(argv[2], &p, 10);
  nc = strtol(argv[3], &p, 10);
  N_pilha = malloc(
      N * sizeof(int)); // alocaçao dinamica do tamanho da pilha para facilitar
  threads = nc + np;
  // Checagem basica
  if (nc <= 0 || np <= 0 || N <= 0) {
    printf("Tamanho da pilha: %d\nNúmero de Threads Totais: %d\nNúmero de "
           "Threads Produtoras: %d\nNúmero de Threads Consumidoras: %d\n",
           N, threads, np, nc);
    printf("Faz sentido isso? Tem que ter no mínimo 1 Thread Produtora e "
           "Consumidora. Ah, o tamanho da pilha tem que ser maior que 0\n");
    return 0;
  }

  printf("Tamanho da pilha: %d\nNúmero de Threads Totais: %d\nNúmero de "
         "Threads Produtoras: %d\nNúmero de Threads Consumidoras: %d\n",
         N, threads, np, nc);

  th = malloc(threads * sizeof(pthread_t)); // criando as threads
  pthread_mutex_init(&mutexN_pilha, NULL);
  sem_init(&semaforo_livre, 0,
           N); // N é qnts estão disponíveis no inicio do código
  sem_init(&semaforo_preenchido, 0,
           0); // 0 é qnts estão preenchidos no inicio do código
  for (int i = 0; i < threads; i++) {
    // criando np produtoras. o resto é consumidora
    if (i < np) {
      pthread_create(&th[i], NULL, &produzir, NULL);
      printf("Produtor criado!\n");
    } else {
      pthread_create(&th[i], NULL, &consumir, NULL);
      printf("Consumidor criado!\n");
    }
  }
  for (int i = 0; i < threads; i++) {
    pthread_join(th[i], NULL);
  }
  finish = clock();
  media = ((double)(finish - start)) / CLOCKS_PER_SEC;
  fprintf(file, "%f\n", media);
  fclose(file);
  return 0;
}
