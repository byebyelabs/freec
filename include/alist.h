// tracing and allocation-list header file
// need both in one place because of codependency

#ifndef ALIST_T
#define ALIST_T 1

#include <stdbool.h>
#include <stdlib.h>

#define MAX_PATH_BUFF 256 /* including \0 */

typedef int ln_num_t;
typedef enum { ALLOC, FREE, DEREF } trace_event_t;
typedef enum {
  USE_BEFORE_MALLOC,
  USE_AFTER_FREE,
  INVALID_FREE,
  DOUBLE_FREE
} memory_violation_t;

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

// Fills the given trace_info_t with the current event type and
// source location
// \param info        pointer to the trace_info_t to populate
// \param event_type  the event type to record
void _fill_trace_info(trace_info_t *info, trace_event_t event_type);

// Dumps error information for the given node and exits with EXIT_FAILURE
// \param node        pointer to the node for which to dump error info
void _dump_error_info_and_exit(alloc_node_t *node,
                               memory_violation_t violation_type);

// Run addr2line and save results in info
// \param info        trace_info_t to fill out
// \param backtrace   result of running backtrace symbols offset
void addr2line(trace_info_t *info, char *backtrace);

// TODO: add documentation
void _find_node(void *mem_ptr, alloc_node_t **node, bool exact_match);

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
