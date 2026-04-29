#include <stdlib.h>

int val = 1;

int main(void) {
  int *p = &val;
  free(p); // invalid free
  return 0;
}
