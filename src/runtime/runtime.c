#include "runtime.h"

// Citation: https://arc.net/l/quote/tmdpqzak
void* real_malloc(size_t size) {
  static malloc_fn_t real_fn = NULL;
  if (real_fn == NULL) {
    real_fn = (malloc_fn_t)dlsym(RTLD_NEXT, "malloc");
    if (real_fn == NULL) {
      fprintf(stderr, "Failed to locate malloc: %s\n", dlerror());
    }
  }

  return real_fn(size);
}

void real_free(void* ptr) {
  static free_fn_t real_fn = NULL;
  if (real_fn == NULL) {
    real_fn = (free_fn_t)dlsym(RTLD_NEXT, "free");
    if (real_fn == NULL) {
      fprintf(stderr, "Failed to locate free: %s\n", dlerror());
    }
  }

  real_fn(ptr);
}

void* malloc(size_t size) {
  printf("malloc called: malloc(%zu)\n", size);
  return real_malloc(size);
}

void free(void* ptr) {
  printf("free called: free(%p)\n", ptr);
  real_free(ptr);
}
