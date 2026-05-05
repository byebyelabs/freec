#ifndef RUNTIME_HEADERS_T
#define RUNTIME_HEADERS_T 1
#include <signal.h>
#include <string.h>

void set_route_custom_malloc_to_real_malloc(void);

void unset_route_custom_malloc_to_real_malloc(void);

void protected_page_access_handler(int signal, siginfo_t* info, void* ctx);

#endif
