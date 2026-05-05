#include "alist.h"
#include "utils.h"

#include "tracing.c"

#include <pthread.h>

static alloc_node_t *alloc_list_head = NULL;
static pthread_mutex_t alloc_list_mutex = PTHREAD_MUTEX_INITIALIZER;

// TODO: change exit(1) to warnings!
// TODO: test ptr comparisons

void add_alloc_event(void *mem_ptr, size_t block_sz) {
  // use real malloc to allocate memory for node
  alloc_node_t *new_node = real_malloc(sizeof(alloc_node_t));
  if (new_node == NULL) {
    log_message("Failed Allocating Memory in add_alloc_event!\n", ERROR);
    exit(1);
  }

  // initialize node
  new_node->alloc_start = mem_ptr;
  new_node->alloc_size = block_sz;
  new_node->next = NULL;

  // fill trace info (last event == this event)
  _fill_trace_info(&new_node->alloc_info, ALLOC);
  new_node->last_event = new_node->alloc_info;

  // add node to linked list; need to lock in case of concurrent access
  pthread_mutex_lock(&alloc_list_mutex);
  if (alloc_list_head == NULL) {
    alloc_list_head = new_node;
  } else {
    new_node->next = alloc_list_head;
    alloc_list_head = new_node;
  }
  pthread_mutex_unlock(&alloc_list_mutex);
}

void add_deref_event(void *mem_ptr) {
  // check if node exists
  alloc_node_t *node = NULL;
  _find_node(mem_ptr, &node, false);

  // use before malloc error
  if (node == NULL) {
    log_message("Use Before Malloc Error!\n", ERROR);
    exit(1);
  }

  // use after free error
  if (node->last_event.event == FREE) {
    log_message("Use After Free Error!\n", ERROR);
    exit(1);
  }

  // fill trace info (last event == this event)
  _fill_trace_info(&node->last_event, DEREF);
}

void add_freed_event(void *mem_ptr) {
  // check if node exists
  alloc_node_t *node = NULL;
  _find_node(mem_ptr, &node, true);

  // invalid free error
  if (node == NULL) {
    log_message("Invalid Free Error!\n", ERROR);
    exit(1);
  }

  // double free error
  if (node->last_event.event == FREE) {
    log_message("Double Free Error!\n", ERROR);
    exit(1);
  }

  // fill trace info (last event == this event)
  _fill_trace_info(&node->last_event, FREE);
}

void _find_node(void *mem_ptr, alloc_node_t **node, bool exact_match) {
  // find node in linked list and update node pointer
  pthread_mutex_lock(&alloc_list_mutex);
  alloc_node_t *current = alloc_list_head;
  while (current != NULL) {
    // get start and end of allocation
    char *start = (char *)current->alloc_start;
    char *end = start + current->alloc_size;
    char *ptr = (char *)mem_ptr;

    // only check for exact match
    if (exact_match && current->alloc_start == mem_ptr) {
      *node = current;
      break;
    }
    // check if memory is within allocation
    else if (!exact_match && start <= ptr && ptr < end) {
      *node = current;
      break;
    }

    current = current->next;
  }
  pthread_mutex_unlock(&alloc_list_mutex);
}
