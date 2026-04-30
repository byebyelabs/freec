#include <execinfo.h>
#include <stdio.h>

#include "alist.h"
#include "utils.h"

#define TRACE_DEPTH 32

void _fill_trace_info(alloc_node_t *node, trace_event_t event_type) {
  if (node == NULL) {
    perror("[tracing]: should be unreachable: node is null");
  }

  node->last_event.event = event_type;

  // Citation:
  //    https://man7.org/linux/man-pages/man3/backtrace.3.html
  //    https://stackoverflow.com/a/5946948,
  //    https://www.geeksforgeeks.org/c/pipe-system-call/
  void *traces[TRACE_DEPTH];
  size_t count = backtrace(traces, TRACE_DEPTH);
  char **funcNames = backtrace_symbols(traces, count);

  for (size_t i = 0; i < count; i++) {
    char funcName[1024];
    snprintf(funcName, sizeof(funcName), "%s\n", funcNames[i]);
    log_message(funcName);
  }

  free(funcNames);
}

void _dump_error_info(alloc_node_t *node) {
  if (node == NULL)
    return;
}
