#include <execinfo.h>
#include <stdio.h>

#include "alist.h"
#include "utils.h"

#define TRACE_DEPTH 32

void _fill_trace_info(alloc_node_t *node, trace_event_t event_type) {
  log_message("[tracing]: _fill_trace_info called\n", DEBUG);
  if (node == NULL) {
    log_message("[tracing]: should be unreachable! node is `null`\n", ERROR);
    exit(EXIT_FAILURE);
  }

  // node->last_event.event = event_type;

  // Citation:
  //    https://man7.org/linux/man-pages/man3/backtrace.3.html
  //    https://stackoverflow.com/a/5946948,
  //    https://www.geeksforgeeks.org/c/pipe-system-call/
  void *traces[TRACE_DEPTH];
  size_t count = backtrace(traces, TRACE_DEPTH);

  // backtrace_symbols makes a malloc call, which is why I added
  // `CURRENTLY_IN_MALLOC` flag to avoid a recursive call
  char **funcNames = backtrace_symbols(traces, count);

  for (size_t i = 0; i < count; i++) {
    char funcName[1024];
    snprintf(funcName, sizeof(funcName), "%s\n", funcNames[i]);
    log_message(funcName, DEBUG);
  }

  real_free(funcNames);
}

void _dump_error_info(alloc_node_t *node) {
  if (node == NULL)
    return;
}
