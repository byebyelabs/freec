#include "utils.h"

void *malloc(size_t size) {
  // TODO: malloc a page
  log_message("malloc called: malloc(zu)\n");
  return real_malloc(size);
}

void free(void *ptr) {
  log_message("free called: free(p)\n");
  real_free(ptr);
  // TODO: instead of freeing, protect page
}

// void protected_page_access_handler(??) {
//   ??
// }
