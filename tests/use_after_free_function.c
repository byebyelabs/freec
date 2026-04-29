#include <stdio.h>
#include <stdlib.h>

int *make_and_destroy(void) {
  int *p = malloc(sizeof(int));
  *p = 1; // legal reference
  free(p);
  return p;
}

int main(void) {
  int *val = make_and_destroy();
  printf("%d\n", *val); // illegal reference
  return 0;
}
