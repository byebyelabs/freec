#define _GNU_SOURCE

#include "alist.h"
#include "utils.h"

// TODO: make it possible to have more pages
// underlying page
void* u_page_start = NULL;
size_t offset = 0;

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

  log_message("malloc called: malloc(zu)\n", DEBUG);
  if (u_page_start == NULL) {
    // if no underlying page
    log_message("making new page\n", DEBUG);
    u_page_start = mmap(NULL, get_page_size(), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    if (u_page_start == MAP_FAILED) {
      perror("!! Failed to mmap underlying page!");
      exit(EXIT_FAILURE);
    }
  }

  // make virtual page
  void* v_page_start = mremap((uint8_t*) u_page_start + offset, 0, get_page_size(), MREMAP_MAYMOVE);
  if (v_page_start == MAP_FAILED) {
    perror("!! Failed to make virtual address!");
  }

  // construct virtual address
  void* v_addr = (uint8_t*) v_page_start + offset;
  offset += size;
  add_alloc_event(v_addr, size);

  return v_addr;
}

void free(void *ptr) {
  if (ROUTE_CUSTOM_MALLOC_TO_REAL_MALLOC) {
    real_free(ptr);
    return;
  }
  log_message("free called: free(p)\n", DEBUG);
  add_freed_event(ptr);
  // if we have gotten past add freed event, we can freely mprotect
  char buffer[100];
  sprintf(buffer, "valid free of %p!\n", ptr);
  log_message(buffer, DEBUG);
  mprotect(ptr, get_page_size(), PROT_NONE);
}

// void protected_page_access_handler(??) {
//   ??
// }
