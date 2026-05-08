#define _GNU_SOURCE

#include "runtime.h"
#include "alist.h"
#include "utils.h"

// #include <stdlib.h>

// TODO: make it possible to have more pages
/**
 * underlying page
 */
void *u_page_start = NULL;
size_t offset = 0;

int ROUTE_CUSTOM_MALLOC_TO_REAL_MALLOC = 0;

void set_route_custom_malloc_to_real_malloc(void) {
  ROUTE_CUSTOM_MALLOC_TO_REAL_MALLOC = 1;
}

void unset_route_custom_malloc_to_real_malloc(void) {
  ROUTE_CUSTOM_MALLOC_TO_REAL_MALLOC = 0;
}

// ---- SIGNAL HANDLING -----------------------------
__attribute__((constructor)) void init() {
  struct sigaction sa;
  memset(&sa, 0, sizeof(struct sigaction));
  sa.sa_sigaction = protected_page_access_handler;
  sa.sa_flags = SA_SIGINFO;

  if (sigaction(SIGSEGV, &sa, NULL) != 0) {
    log_message("!! sigaction failed", ERROR);
    real_exit(EXIT_FAILURE);
  }
}

void protected_page_access_handler(int signal, siginfo_t *info, void *ctx) {
  (void)signal;
  (void)ctx;
  add_deref_event(info->si_addr);
}

// ---- CUSTOM ALLOC. FUNCS -------------------------
void *malloc(size_t size) {
  if (ROUTE_CUSTOM_MALLOC_TO_REAL_MALLOC) {
    return real_malloc(size);
  }

  log_message("malloc called: malloc(zu)\n", DEBUG);
  if (u_page_start == NULL) {
    // if no underlying page
    log_message("making new page\n", DEBUG);
    u_page_start = mmap(NULL, get_page_size(), PROT_READ | PROT_WRITE,
                        MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    if (u_page_start == MAP_FAILED) {
      log_message("!! Failed to mmap underlying page!", ERROR);
      real_exit(EXIT_FAILURE);
    }
  }

  // make virtual page
  void *v_page_start = mremap((uint8_t *)u_page_start, 0,
                              get_page_size(), MREMAP_MAYMOVE);
  if (v_page_start == MAP_FAILED) {
    log_message("!! Failed to make virtual address!", ERROR);
    real_exit(EXIT_FAILURE);
  }

  // construct virtual address
  void *v_addr = (uint8_t *)v_page_start + offset;
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

// ---- EXIT HANDLING -------------------------------
void exit(int status) {
    check_for_unfreed_memory();
    real_exit(status);

    // to avoid compiler warning since this 
    // function has a [[noreturn]] attribute
    while(true) {}
}

void _Exit(int status) {
    check_for_unfreed_memory();
    real__Exit(status);

    // to avoid compiler warning since this 
    // function has a [[noreturn]] attribute
    while(true) {}
}

void abort(void) {
    check_for_unfreed_memory();
    real_abort();

    // to avoid compiler warning since this 
    // function has a [[noreturn]] attribute
    while(true) {}
}
