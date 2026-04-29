#include <stdlib.h>

int main(void) {
  int *arr = malloc(10 * sizeof(int));
  free(arr);
  arr[0] = 99;
  return 0;
}
