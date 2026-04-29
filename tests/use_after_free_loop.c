#include <stdio.h>
#include <stdlib.h>

int main(void) {
  int *p = malloc(5 * sizeof(int));
  for (int i = 0; i < 5; i++) {
    p[i] = i;
  }

  free(p);

  for (int i = 0; i < 5; i++) {
    printf("%d\n", p[i]); // use 5 times after free (5 issues)
  }

  return 0;
}
