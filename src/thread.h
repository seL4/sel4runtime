/*
 * seL4 thread representation.
 */

#include <sel4/sel4.h>

#pragma once

// seL4 threads.
typedef struct {
    int errno;
    seL4_IPCBuffer *ipc_buffer;
    seL4_CPtr ipc_buffer_page;
    seL4_CPtr tcb;
    seL4_CPtr cnode;
    seL4_CPtr vspace;
    seL4_CPtr asid_pool;
} thread_t;

/*
 * Obtain a reference to the current thread.
 */
thread_t *__sel4runtime_thread_self(void);
