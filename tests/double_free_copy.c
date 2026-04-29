#include <stdlib.h>

int main(void) {
  int *p = malloc(sizeof(int));
  int *q = p; // copy
  *p = 7;
  free(p);
  free(q); // freeing copy
  return 0;
}
