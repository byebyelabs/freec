#include "runtime.h"
#include "utils.h"

int ROUTE_CUSTOM_MALLOC_TO_REAL_MALLOC = 0;

void set_route_custom_malloc_to_real_malloc(void) {
    ROUTE_CUSTOM_MALLOC_TO_REAL_MALLOC = 1;
}

void unset_route_custom_malloc_to_real_malloc(void) {
    ROUTE_CUSTOM_MALLOC_TO_REAL_MALLOC = 0;
}

void *malloc(size_t size) {
  if (ROUTE_CUSTOM_MALLOC_TO_REAL_MALLOC) {
    return real_malloc(size);
  }

  // TODO: malloc a page
  log_message("malloc called: malloc(zu)\n", DEBUG);

  return real_malloc(size);
}

void free(void *ptr) {
  log_message("free called: free(p)\n", DEBUG);
  real_free(ptr);
  // TODO: instead of freeing, protect page
}

// void protected_page_access_handler(??) {
//   ??
// }
