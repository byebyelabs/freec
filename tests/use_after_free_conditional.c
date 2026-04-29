#include <stdio.h>
#include <stdlib.h>

int main(void) {
  int *p = malloc(sizeof(int));
  *p = 10;

  int cond = 1;
  if (cond) {
    free(p);
  }

  printf("%d\n", *p);
  return 0;
}
