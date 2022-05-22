#include <math.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

// rodar com gcc produtor_consumidor.c -o teste -lpthread -lrt -lm && ./teste 32
// 1 1

/*
  problemas :

    acesso a memoria compartilhada --> semaforo mutex
    o que fazer quando a memória esta cheia/vazia? Checar se esta vazio ou cheio
  --> semaforo contador

*/

/*
  definir n = tamanho do N_pilha ===============================================
  OK

  definir nc e np = numero de threads de cada tipo
  =============================== OK

  produtor gera de 1 - 10^7 e bota na mem
  ======================================== OK

  consumidor pega o numero, esvazia o local e verifica se é primo. imprime no
  terminal o resultado
  ============================================================= OK

  ordem na mem = semaforos
  =========================================================== OK

  se pode entrar no N_pilha ou nao = semaforo contador
  ================================= OK

*/

pthread_mutex_t mutexN_pilha; // semaforo mutex p adicionar/apagar da pilha
sem_t semaforo_livre;         // semaforo contador de posições livres
sem_t semaforo_preenchido;    // semaforo contador de posições ocupadas

int *N_pilha = NULL; // será usado como uma pilha --> facilita o processo
int contador =
    0; // usado para marcar em qual indice deve ser colocado/lido o número
int ja_processados = 0; // para limitar o numero de valores consumidos
int ja_produzidos = 0;

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

// precisa verificar quantos numeros ja foram processados, senao vai rodar
// infinitamente.
void *produzir(void *args) {
  srand(time(NULL));
  while (ja_processados < (int)pow(10, 1)) {
    int r_numb = rand() % (int)pow(10, 7) + 1;
    printf("Produzido: %d\n", r_numb);
    sem_wait(
        &semaforo_livre); // só produz se tiver espaço livre. Fica esperando
    pthread_mutex_lock(&mutexN_pilha);
    N_pilha[contador] = r_numb;
    contador++;
    ja_produzidos++;
    pthread_mutex_unlock(&mutexN_pilha);
    sem_post(&semaforo_preenchido); // avisa que produziu 1
  }
}

// precisa verificar quantos numeros ja foram processados
void *consumir(void *args) {
  while (ja_processados < (int)pow(10, 1)) {
    sem_wait(&semaforo_preenchido); // só produz se tiver espaço preenchido.
                                    // Fica esperando
    pthread_mutex_lock(&mutexN_pilha);
    int ni = N_pilha[contador - 1];
    is_prime(ni); // talvez tirar do semaforo
    contador--;
    ja_processados++;
    pthread_mutex_unlock(&mutexN_pilha);
    sem_post(&semaforo_livre); // avisa que consumiu
  }
  // exit(0); // Quando processar 10^5, mata o processo. Roubado dms? -->
  // impossivel pegar o tempo
}

int main(int argc, char *argv[]) {
  char *p; // pega valores do terminal
  int N, threads, nc, np;
  clock_t start, finish;
  int i = 10;
  double media = 0;
  FILE *file = fopen("resultados.csv", "a");
  fprintf(file, "Combinacao, N, Tempo\n");

  N = strtol(argv[1], &p, 10);
  np = strtol(argv[2], &p, 10);
  nc = strtol(argv[3], &p, 10);
  fprintf(file, "%d %d, %d, ", np, nc, N);
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

  while (i > 0) {
    pthread_t th[threads]; // criando as threads
    pthread_mutex_init(&mutexN_pilha, NULL);
    sem_init(&semaforo_livre, 0,
             N); // N é qnts estão disponíveis no inicio do código
    sem_init(&semaforo_preenchido, 0,
             0); // 0 é qnts estão preenchidos no inicio do código
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
    sem_destroy(&semaforo_livre);
    sem_destroy(&semaforo_preenchido);
    pthread_mutex_destroy(&mutexN_pilha);
    finish = clock();
    media += ((double)(finish - start)) / CLOCKS_PER_SEC;
    // fprintf(file, "%f\n", ((double)(finish - start)) / CLOCKS_PER_SEC);
    i--;
  }
  fprintf(file, "%f\n", media / 10);
  fclose(file);
  return 0;
}
