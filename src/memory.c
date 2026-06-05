#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <unistd.h>

// linked list to track allocations
typedef struct Allocation {
    void *ptr; // returned by malloc
    size_t size; // requested alloc size
    struct Allocation *next; // next item in allocation list
} Allocation;

static Allocation *allocations = NULL; // head of linked list

// for statistics chosen values we want to track
static size_t malloc_calls = 0;
static size_t free_calls = 0;
static size_t current_bytes = 0;
static size_t peak_bytes = 0; // tracks highest memory allocation over time

// real libc function pointers
static void *(*real_malloc)(size_t) = NULL;
static void (*real_free)(void *) = NULL;

// guards to prevent loops
static int initialized = 0;
static int in_hook = 0;

static void print_report(void); // prototype/forward declaration

static void init_hooks(void) { // initialises real malloc/free function
    if (initialized) {
        return;
    }
    initialized = 1;

    real_malloc = dlsym(RTLD_NEXT, "malloc");
    real_free   = dlsym(RTLD_NEXT, "free");

    atexit(print_report);
}

// malloc() hijack
void *malloc(size_t size) {
    if (!initialized) {
        init_hooks();
    }
    if (in_hook) { // prevent recursion
        return real_malloc(size);
    }

    in_hook = 1;
    malloc_calls++;
    void *ptr = real_malloc(size); // allocate asked memory

    if (ptr) {
        Allocation *node =real_malloc(sizeof(Allocation)); // add allocation to tracking linked list

        node->ptr = ptr;
        node->size = size;
        node->next = allocations;
        allocations = node;
        current_bytes += size;

        if (current_bytes > peak_bytes){
            peak_bytes = current_bytes;
        }
    }
    in_hook = 0;
    return ptr;
}

// free() hijack
void free(void *ptr) {
    if (!initialized) {
        init_hooks();
    }
    if (ptr == NULL) {
        return;
    }

    if (in_hook) { // prevent recursion/loop
        real_free(ptr);
        return;
    }

    in_hook = 1;
    free_calls++;

    Allocation *prev = NULL;
    Allocation *cur = allocations;

    while (cur) {
        if (cur->ptr == ptr) { // update linked list / remove memory pointer that was just freed in linked list
            current_bytes -= cur->size;
            if (prev) {
                prev->next = cur->next;
            } else {
                allocations = cur->next;
            }
            real_free(cur); // free our pointer from linked list
            break;
        }
        prev = cur;
        cur = cur->next;
    }

    real_free(ptr); // free actual memory that wants to be freed
    in_hook = 0;
}

// helper function to make a formatted report table with our statistics
static void print_report() {
    fprintf(stderr, "\n==============================\n" ">  Memory Allocation Report  <\n" "==============================\n");

    fprintf(stderr, "malloc() calls : %zu\n" "free() calls   : %zu\n" "current bytes: %zu\n" "peak bytes   : %zu\n\n", malloc_calls, free_calls, current_bytes, peak_bytes);

    Allocation *cur = allocations;

    if (!cur) {
        fprintf(stderr, "No memory leaks detected.\n");
        return;
    }

    fprintf(stderr, "Memory leaks:\n\n");

    size_t total_leaked = 0;

    while (cur) {
        fprintf(stderr, "LEAK: ptr=%p size=%zu bytes\n", cur->ptr, cur->size);

        total_leaked += cur->size;
        cur = cur->next;
    }
    
    fprintf(stderr, "\nTotal leaked: %zu bytes\n", total_leaked);
}