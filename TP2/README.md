gcc produtor_consumidor.c -o teste -lpthread -lrt -lm && ./teste 32 1 1

  finish = clock();
  FILE *file = fopen("resultados.csv", "a");
  fprintf(file, "%f\n", ((double)(finish - start)) / CLOCKS_PER_SEC );