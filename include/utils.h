#ifndef FREEC_UTILS_H
#define FREEC_UTILS_H 1

#include <dlfcn.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>

typedef void *(*malloc_fn_t)(size_t size);
typedef void (*free_fn_t)(void *ptr);

void *real_malloc(size_t size);
void real_free(void *ptr);

/**
 * Print a message directly to standard error without invoking malloc or free.
 * \param message   A null-terminated string that contains the message to be
 * printed
 */
typedef enum { DEBUG, WARNING, ERROR } severity_t;
void log_message(char *message, severity_t severity);

typedef long page_size_t;
/**
 * Get the page size.
 */
page_size_t get_page_size(void);

#endif
