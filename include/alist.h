#ifndef ALIST_T
#define ALIST_T 1

#define MAX_PATH_BUFF 256 /* including \0 */

typedef int ln_num_t;
typedef enum { ALLOC, FREE, DEREF } trace_event_t;
 
typedef struct {
    ln_num_t line;
    char[MAX_PATH_BUFF] file_path;
    trace_event_t event;
} trace_info_t;

typedef struct alloc_node {
    struct alloc_node* next;
    void* alloc_start;
    size_t alloc_size;
    trace_info_t alloc_info;
    trace_info_t last_event;
} alloc_node_t;


// TODO: add documentation
void fill_trace_info(alloc_node_t* node, trace_event_t event_type);
void dump_trace_info(alloc_node_t* node);

// TODO: add documentation
void add_alloc_event(void* mem_ptr, size_t block_sz);
void add_deref_event(void* mem_ptr);
void add_freed_event(void* mem_ptr);

#endif