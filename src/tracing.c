#include <execinfo.h>
#include <stdio.h>

#include "alist.h"
#include "runtime.h"
#include "utils.h"

#define TRACE_DEPTH 32

// Number of places to go back so the recorded loc points at
// user code
#define BACKTRACE_SKIP 3

void _fill_trace_info(trace_info_t *info, trace_event_t event_type) {
  log_message("[tracing]: _fill_trace_info called\n", DEBUG);
  if (info == NULL) {
    log_message("[tracing]: should be unreachable! info is `null`\n", ERROR);
    exit(EXIT_FAILURE);
  }

  info->event = event_type;
  info->line = 0;
  info->file_path[0] = '\0';

  // Citation:
  //    https://man7.org/linux/man-pages/man3/backtrace.3.html
  //    https://stackoverflow.com/a/5946948,
  //    https://www.geeksforgeeks.org/c/pipe-system-call/
  void *traces[TRACE_DEPTH];
  // both backtrace funcs make a malloc call
  set_route_custom_malloc_to_real_malloc();
  size_t count = backtrace(traces, TRACE_DEPTH);
  if (count == 0) {
    unset_route_custom_malloc_to_real_malloc();
    return;
  }

  char **funcNames = backtrace_symbols(traces, count);
  unset_route_custom_malloc_to_real_malloc();

  if (funcNames == NULL) { return; }

  size_t user_code_loc =
      (count > BACKTRACE_SKIP) ? BACKTRACE_SKIP : count - 1;
  snprintf(info->file_path, sizeof(info->file_path), "%s",
           funcNames[user_code_loc]);

  log_message("[tracing]: filled trace location: ", DEBUG);
  log_message(info->file_path, DEBUG);
  log_message("\n", DEBUG);

  real_free(funcNames);
}

void _dump_error_info(alloc_node_t *node) {
  if (node == NULL) { return; }

  log_message("[tracing]: _dump_error_info needs to dump here\n", DEBUG);
}
