#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#include "alist.h"
#include "runtime.h"
#include "utils.h"

#define TRACE_DEPTH 32
#define USER_CODE_LOCATION_IN_TRACE 3

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
  
  // both backtrace funcs and addr2line make a malloc call
  set_route_custom_malloc_to_real_malloc();
  size_t count = backtrace(traces, TRACE_DEPTH);
  if (count == 0) {
    unset_route_custom_malloc_to_real_malloc();
    return;
  }

  char **funcNames = backtrace_symbols(traces, count);

  if (funcNames == NULL) { return; }
  addr2line(info, funcNames[USER_CODE_LOCATION_IN_TRACE]);

  unset_route_custom_malloc_to_real_malloc();

  log_message("[tracing]: filled trace location: ", DEBUG);
  log_message(info->file_path, DEBUG);
  log_message("\n", DEBUG);

  real_free(funcNames);
}

void _dump_error_info(alloc_node_t *node) {
  if (node == NULL) { return; }

  log_message("[tracing]: _dump_error_info needs to dump here\n", DEBUG);
}

void addr2line(trace_info_t *info, char *backtrace) {
  log_message("[tracing] addr2line called\n", DEBUG);
    
  // backtrace looks like: `<obj_loc>(+<offset>) [<instr_addr>]`
  // need to extract obj_loc and offset
  char *obj_loc = backtrace;
  while (*backtrace != '(') backtrace++;

  *backtrace = '\0';
  backtrace += 2;
  
  char *offset = backtrace;
  while (*backtrace != ')') backtrace++;
  *backtrace = '\0';

  // FOSNP: fear of snprintf
  log_message("[tracing] addr2line obj_loc: ", DEBUG);
  log_message(obj_loc, DEBUG);
  log_message(" offset: ", DEBUG);
  log_message(offset, DEBUG);
  log_message("\n", DEBUG);

  // create pipe to capture result of addr2line
  // Citation: https://man7.org/linux/man-pages/man2/pipe.2.html
  int pipefd[2];
  
  // next, call `addr2line -e <obj_loc> -i <offset>` in a different process
  pid_t child_pid = fork();
  if(child_pid == -1) {
    log_message("[tracing] fork failed in addr2line", ERROR);
    exit(EXIT_FAILURE);
  }

  if(child_pid == 0) {
    // close unused read for child
    close(pipefd[0]);

    // print to pipefd instead of STDOUT
    // Citation: https://man7.org/linux/man-pages/man2/dup.2.html
    dup2(stdout, pipefd[1]);
      
    char* a2l_args[] = { "addr2line", "-e", obj_loc, "-i", offset, NULL };
    if(execvp("LD_PRELOAD='' addr2line", a2l_args)) {
      log_message("[tracing]: execvp failed\n", ERROR);
      exit(EXIT_FAILURE);
    }
  } 
  else {
      // Parent process: wait for child then copy result to info
      wait(NULL);
      
      // close unused write for parent
      close(pipefd[0]);

      // read result of addr2line
      read(pipefd[0], info->file_path, MAX_PATH_BUFF - 1);      
  }
  
  info->line = 0;
  info->file_path[MAX_PATH_BUFF - 1] = '\0';
}
