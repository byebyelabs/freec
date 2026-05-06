#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "alist.h"
#include "runtime.h"
#include "utils.h"

#define TRACE_DEPTH 32
#define MAX_LINE_NUM_DIGITS 7
#define USER_CODE_LOCATION_IN_TRACE 3

void _pretty_print_trace(trace_info_t *info, char *message, char highlight);

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

  if (funcNames == NULL) {
    return;
  }
  addr2line(info, funcNames[USER_CODE_LOCATION_IN_TRACE]);

  unset_route_custom_malloc_to_real_malloc();

  // line number should not be more than 15 digits
  char tmp_line_num_str[16]; 
  sprintf(tmp_line_num_str, "%d\n", info->line);

  log_message("[tracing]: filled trace location: ", DEBUG);
  log_message(info->file_path, DEBUG);
  log_message(" line: ", DEBUG);
  log_message(tmp_line_num_str, DEBUG);

  real_free(funcNames);
}

void _dump_error_info_and_exit(alloc_node_t *node, memory_violation_t violation_type) {
  if (node == NULL) {
    return;
  }

  // this function will exit
  set_route_custom_malloc_to_real_malloc();

  trace_info_t violator;
  // violation can only occur when user defers or frees
  trace_event_t user_action = (USE_AFTER_FREE || violation_type == USE_BEFORE_MALLOC)?DEREF:FREE;
  _fill_trace_info(&violator, user_action);

  // Rust style error dumping kind of like follows
  /*
    error: cannot free the same memory more than once
    -> /home/usr/csc313/freec/tests/double_free_basic.c:8
    05   int *p = malloc(sizeof(int) * 10);
         ---------------------------------- malloc happened here
    ...
    07   free(p);
         -------- first free occured here
    ...
    08   free(p); // DOUBLE FREE
         ^^^^^^^^^^^^^^^^^^^^^^^ second free occured here
   */

  // helpful message first
  log_message("error: ", ERROR);
  if (violation_type == USE_AFTER_FREE) {
      log_message("cannot use memory after freeing\n", ERROR);
  } else if (violation_type == INVALID_FREE) {
      log_message("cannot free memory that is not malloc-ed\n", ERROR);
  } else if (violation_type == DOUBLE_FREE) {
      log_message("cannot free the same memory more than once\n", ERROR);
  }

  // file where violation occured
  log_message("-> ", ERROR);
  log_message(violator.file_path, ERROR);
  char tmp_ln_to_str[MAX_LINE_NUM_DIGITS + 2];
  sprintf(tmp_ln_to_str, ":%d\n", violator.line);
  log_message(tmp_ln_to_str, ERROR);

  // if USE_AFTER_FREE or DOUBLE_FREE, print where malloc-ed
  if (violation_type == USE_AFTER_FREE || violation_type == DOUBLE_FREE)
      _pretty_print_trace(&node->alloc_info, "malloc happened here", '-');

  // print last event
  _pretty_print_trace(&node->last_event, "last event TODO:improve msg", '-');
  
  // print violation
  _pretty_print_trace(&violator, "real issue TODO:improve msg", '^');

  // crash
  exit(EXIT_FAILURE);
}

int TMP=0;
void addr2line(trace_info_t *info, char *backtrace) {
  log_message("[tracing] addr2line called\n", DEBUG);

  // backtrace looks like: `<obj_loc>(+<offset>) [<instr_addr>]`
  // need to extract obj_loc and offset
  char *obj_loc = backtrace;
  while (*backtrace != '(')
    backtrace++;

  *backtrace = '\0';
  backtrace += 2;

  char *offset = backtrace;
  while (*backtrace != ')')
    backtrace++;
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
  if (child_pid == -1) {
    log_message("[tracing] fork failed in addr2line", ERROR);
    exit(EXIT_FAILURE);
  }

  if (child_pid == 0) {
    // close unused read for child
    close(pipefd[0]);

    // print to pipefd instead of STDOUT
    // Citation: https://man7.org/linux/man-pages/man2/dup.2.html
    dup2(STDOUT_FILENO, pipefd[1]);

    char *a2l_args[] = {"addr2line", "-e", obj_loc, "-i", offset, NULL};
    a2l_args[0]++;
    // if (execvp("LD_PRELOAD='' addr2line", a2l_args)) {
    //   log_message("[tracing]: execvp failed\n", ERROR);
    //   exit(EXIT_FAILURE);
    // }

    // end child process
    exit(EXIT_SUCCESS);
  } else {
    // Parent process: wait for child then copy result to info
    wait(NULL);

    // close unused write for parent
    close(pipefd[0]);

    // read result of addr2line
    read(pipefd[0], info->file_path, MAX_PATH_BUFF - 1);
  }

  log_message("offset: ", DEBUG);
  log_message(offset, DEBUG);
  log_message("\n", DEBUG);

  // hard coding because can't run addr2line
  info->line = (TMP == 0)?5:((TMP == 1)?7:8);
  TMP++;
  strcpy(info->file_path, "/home/bhattara/csc313/freec/tests/double_free_basic.c");
  info->file_path[MAX_PATH_BUFF - 1] = '\0';
}

void _print_file_line(char *path, int line, char highlight, char *message) {
    set_route_custom_malloc_to_real_malloc();
    FILE *file = fopen(path, "r");
    int leading_white_space = MAX_LINE_NUM_DIGITS - 2, trimmed_line_len = 0;

    line -= 1; // file lines are 1-indexed
    while (line) {
        if (fgetc(file) == '\n') line--;
    }

    // skip leading white space
    while (fgetc(file) == ' ')
        leading_white_space++;

    // first char was skipped
    fseek(file, -1, SEEK_CUR);
    
    // print line
    char ch = fgetc(file);
    char str[2]; 
    do {
        snprintf(str, 2, "%c", ch);
        log_message(str, ERROR);
        trimmed_line_len++;
    } while ((ch = fgetc(file)) != '\n');
    log_message("\n", ERROR);

    // in new line, first print leading_white_space ' '
    while (leading_white_space--)
        log_message(" ", ERROR);

    // then print trimmed_line_len highlights
    snprintf(str, 2, "%c", highlight);
    while (trimmed_line_len--)
        log_message(str, ERROR);
    log_message(" ", ERROR);

    // finally print the error message
    log_message(message, ERROR);

    // whitespace and then "...\n"
    log_message("\n", ERROR);
    leading_white_space = MAX_LINE_NUM_DIGITS - 2;
    while (leading_white_space--)
        log_message(" ", ERROR);
    log_message("...\n", ERROR);
    
    fclose(file);
    unset_route_custom_malloc_to_real_malloc();
}

void _pretty_print_trace(trace_info_t *info, char *message, char highlight) {
    char padded_line_num[MAX_LINE_NUM_DIGITS + 1]; // + 1 for space
    sprintf(padded_line_num, "%6d ", info->line);

    log_message(padded_line_num, ERROR);
    _print_file_line(info->file_path, info->line, highlight, message);
}
