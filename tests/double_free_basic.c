// BUG: double free
#include <stdlib.h>

int main(void) {
  int *p = malloc(sizeof(int) * 10);
  *p = 1;
  free(p);
  free(p); // DOUBLE FREE
  return 0;
}
