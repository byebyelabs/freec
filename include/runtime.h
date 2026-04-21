#ifndef FREEC_RUNTIME_H
#define FREEC_RUNTIME_H 1

#include <dlfcn.h>
#include <stdio.h>
#include <unistd.h>
#include <stddef.h>

typedef void* (*malloc_fn_t)(size_t size);
typedef void  (*free_fn_t)(void *ptr);

void *freec_malloc(size_t size);
void  freec_free(void *ptr);

void freec_runtime_report(void);

#endif
