#include <stdlib.h>

int main(void) {
  int *arr = malloc(sizeof(int) * 10);
  int *mid = arr + 2;
  free(mid); // invalid free
  return 0;
}
