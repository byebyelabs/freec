#include "utils.h"

int USER_CURRENTLY_IN_MALLOC = 0;

void *malloc(size_t size) {
  if (USER_CURRENTLY_IN_MALLOC) {
    // something made a recursive call to malloc.
    // it must be our own code/libs we use. make real malloc call.
    return real_malloc(size);
  }

  // malloc has started
  USER_CURRENTLY_IN_MALLOC = 1;

  // TODO: malloc a page
  log_message("malloc called: malloc(zu)\n", DEBUG);

  // malloc has finished
  USER_CURRENTLY_IN_MALLOC = 0;
  return real_malloc(size);
}

void free(void *ptr) {
  log_message("free called: free(p)\n", DEBUG);
  real_free(ptr);
  // TODO: instead of freeing, protect page
}

void protected_page_access_handler(??) {
  ??
}
