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

typedef void (*exit_fn_t)(int status);
typedef void (*abort_fn_t)(void);

void *real_malloc(size_t size);
void real_free(void *ptr);

void real_exit(int status);
void real__Exit(int status);
void real_abort(void);

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

// Citation: https://stackoverflow.com/a/51947317
#define RED_TEXT_START "\033[31m"
#define BLUE_TEXT_START "\033[34m"
#define COLORED_TEXT_END "\033[0m"
#define RED_MSG(X) (RED_TEXT_START X COLORED_TEXT_END)
#define BLUE_MSG(X) (BLUE_TEXT_START X COLORED_TEXT_END)

#endif
