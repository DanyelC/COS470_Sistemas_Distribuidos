#include <stdio.h>
#include <math.h>
#include <stdlib.h>


void consumir(int ni){
  for (int i = 2; i <= sqrt(ni); i++) {
    if (ni % i == 0) {
      printf("O %d não é primo.\n", ni);
      return;
    }
  }
  if (ni <= 1){
    printf("O %d não é primo.\n", ni);
    return;
  }
  printf("O %d é primo. WOW!\n", ni);
}

int main(int argc, char *argv[]) {
  char *p;
  for(int i=1;i<argc;++i){
    long n = strtol(argv[i], &p, 10);
    consumir(n);
  }
  return 0;
}