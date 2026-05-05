#define _GNU_SOURCE

#ifndef FREEC_UTILS_H
#define FREEC_UTILS_H 1

#include <dlfcn.h>
#include <stddef.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stdint.h>

#include "alist.h"

typedef void *(*malloc_fn_t)(size_t size);
typedef void (*free_fn_t)(void *ptr);

void *real_malloc(size_t size);
void real_free(void *ptr);

/**
 * Print a message directly to standard error without invoking malloc or free.
 * \param message   A null-terminated string that contains the message to be
 * printed
 */
void log_message(char *message);

typedef long page_size_t;
/**
 * Get the page size.
 */
page_size_t get_page_size();

#endif
