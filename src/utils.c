#include "utils.h"

#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>

page_size_t _PAGE_SIZE = -1;
page_size_t get_page_size() {
  if (_PAGE_SIZE == -1) {
    _PAGE_SIZE = sysconf(_SC_PAGESIZE);
  }
  return _PAGE_SIZE;
}

void log_message(char *message) {
  // Get the message length
  size_t len = 0;
  while (message[len] != '\0') {
    len++;
  }

  // Write the message
  if ((size_t)write(STDERR_FILENO, message, len) != len) {
    // Write failed. Try to write an error message, then exit
    char fail_msg[] = "logging failed\n";
    write(STDERR_FILENO, fail_msg, sizeof(fail_msg));
    exit(2);
  }
}

// Citation: https://arc.net/l/quote/tmdpqzak
void *real_malloc(size_t size) {
  static malloc_fn_t real_fn = NULL;
  if (real_fn == NULL) {
    real_fn = (malloc_fn_t)dlsym(RTLD_NEXT, "malloc");
    if (real_fn == NULL) {
      fprintf(stderr, "Failed to locate malloc: %s\n", dlerror());
    }
  }

  return real_fn(size);
}

void real_free(void *ptr) {
  static free_fn_t real_fn = NULL;
  if (real_fn == NULL) {
    real_fn = (free_fn_t)dlsym(RTLD_NEXT, "free");
    if (real_fn == NULL) {
      fprintf(stderr, "Failed to locate free: %s\n", dlerror());
    }
  }

  real_fn(ptr);
}
