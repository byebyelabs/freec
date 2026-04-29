#include <stdlib.h>

int main(void) {
  int x = 1;
  int *p = &x;
  free(p); // invalid since x is stack allocated
  return 0;
}
