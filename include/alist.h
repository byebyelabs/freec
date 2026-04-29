// tracing and allocation-list header file
// need both in one place because of codependency

#ifndef ALIST_T
#define ALIST_T 1

#include <stdlib.h>

#define MAX_PATH_BUFF 256 /* including \0 */

typedef int ln_num_t;
typedef enum { ALLOC, FREE, DEREF } trace_event_t;

typedef struct {
  ln_num_t line;
  char file_path[MAX_PATH_BUFF];
  trace_event_t event;
} trace_info_t;

typedef struct alloc_node {
  struct alloc_node *next;
  void *alloc_start;
  size_t alloc_size;
  trace_info_t alloc_info;
  trace_info_t last_event;
} alloc_node_t;

// Adds trace info to given node with given event type
// \param node        pointer to the node to fill
// \param event_type  the type of event to fill in the node's last_event field
void _fill_trace_info(alloc_node_t *node, trace_event_t event_type);

// Dumps error information for the given node
// \param node        pointer to the node for which to dump error info
void _dump_error_info(alloc_node_t *node);

// TODO: add documentation
void add_alloc_event(void *mem_ptr, size_t block_sz);

// TODO: add documentation
// IF:
//  - LAST EVENT == "FREE": "use after free"
//  - NODE == NONE:         "use before malloc"
//
void add_deref_event(void *mem_ptr);

// TODO: add documentation
// IF:
//  - LAST EVENT == "FREE": "double free"
void add_freed_event(void *mem_ptr);

#endif
