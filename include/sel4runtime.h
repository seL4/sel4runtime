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
 * The seL4 run-time interface.
 *
 * This provides an interface to the values managed by sel4runtime.
 */
#include <stddef.h>
#include <sel4/sel4.h>

#pragma once

/*
 * Get the name of the process.
 *
 * Returns the basename of the first argument in argv.
 */
char const *sel4runtime_process_name(void);

/*
 * Get the IPC buffer for the current thread.
 */
seL4_IPCBuffer *sel4runtime_ipc_buffer(void);

/*
 * Get the CPtr for the page of the IPC buffer for the current thread.
 */
seL4_CPtr sel4runtime_thread_ipc_buffer(void);

/*
 * Get the CPtr for the TCB of the current thread.
 */
seL4_CPtr sel4runtime_thread_tcb(void);

/*
 * Get the CPtr for the CNode of the current thread.
 */
seL4_CPtr sel4runtime_thread_cnode(void);

/*
 * Get the CPtr of the VSpace of the current thread.
 */
seL4_CPtr sel4runtime_thread_vspace(void);

/*
 * Get the CPtr of the ASID pool of the current thread.
 */
seL4_CPtr sel4runtime_thread_asid_pool(void);
seL4_IPCBuffer *seL4_GetIPCBuffer(void);

/*
 * Get the bootinfo pointer if the process was provided a bootinfo
 * cspace descriptor.
 */
const seL4_BootInfo *sel4runtime_bootinfo(void);

/*
 * Get the size in bytes needed to store the thread's TLS.
 */
size_t sel4runtime_get_tls_size(void);

/*
 * Check if the TLS for the initial thread is enabled.
 *
 * If the TLS for the initial thread was too large to fit inside the
 * static region of the runtime, this will return false and
 * sel4runtime_move_initial_tls should be used to initialise it into a
 * new location.
 */
int sel4runtime_initial_tls_enabled(void);

/*
 * Write the TLS image for a new thread into the provided memory image.
 * The TLS for the initial thread must be enabled before this is called.
 *
 * Copies the cnode, vspace, and asid_pool from the calling thread.
 *
 * @returns the pointer to the TLS that should be used to call
 *          seL4_TCB_SetTLSBase or NULL on error.
 */
void *sel4runtime_write_tls_image(
    void *tls_memory,
    seL4_CPtr tcb,
    seL4_IPCBuffer *ipc_buffer,
    seL4_CPtr ipc_buffer_page
);

/*
 * Write the TLS image for a new thread into the provided memory image.
 * The TLS for the initial thread must be enabled before this is called.
 *
 * @returns the pointer to the TLS that should be used to call
 *          seL4_TCB_SetTLSBase or NULL on error.
 */
void *sel4runtime_write_tls_image_extended(
    void *tls_memory,
    seL4_CPtr tcb,
    seL4_IPCBuffer *ipc_buffer,
    seL4_CPtr ipc_buffer_page,
    seL4_CPtr cnode,
    seL4_CPtr vspace,
    seL4_CPtr asid_pool
);

/*
 * Move the TLS for the current thread into a new memory location.
 *
 * Particularly useful for enabling TLS in the initial process where the
 * TLS image exeeded the size of the static TLS region in the runtime.
 *
 * @returns the pointer to the TLS that should be used to call
 *          seL4_TCB_SetTLSBase.
 */
void *sel4runtime_move_initial_tls(void *tls_memory);
