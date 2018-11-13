/*
 * Copyright 2018, Data61
 * Commonwealth Scientific and Industrial Research Organisation (CSIRO)
 * ABN 41 687 119 230.
 *
 * This software may be distributed and modified according to the terms of
 * the BSD 2-Clause license. Note that NO WARRANTY is provided.
 * See "LICENSE_BSD2.txt" for details.
 *
 * @TAG(DATA61_BSD)
 */
/*
 * seL4 thread representation.
 */

#include <sel4/sel4.h>

#pragma once

// seL4 threads.
typedef struct {
    void *self;
    void *tls;
    void *tls_region;
    int errno;
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
