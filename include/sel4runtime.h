/*
 * Copyright 2019, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
/*
 * The seL4 run-time interface.
 *
 * This provides an interface to the values managed by sel4runtime.
 */
#include <sel4runtime/stddef.h>
#include <sel4runtime/stdint.h>
#include <sel4/sel4.h>
#include <sel4runtime/thread.h>
#include <sel4runtime/auxv.h>

#pragma once

/*
 * Get the name of the process.
 *
 * Returns the basename of the first argument in argv.
 */
char const *sel4runtime_process_name(void);

/*
 * Get the argument vector passed to main.
 */
char const *const *sel4runtime_argv(void);

/*
 * Get the length of argument vector passed to main.
 */
int sel4runtime_argc(void);

/*
 * Get the list of environment variables passed to main.
 */
char const *const *sel4runtime_envp(void);

/*
 * Get the list of auxiliary vectors.
 */
auxv_t const *sel4runtime_auxv(void);

/*
 * Get the address of the TLS base register.
 */
static inline sel4runtime_uintptr_t sel4runtime_get_tls_base(void);

/*
 * Set the address of the TLS base register.
 */
static inline void sel4runtime_set_tls_base(sel4runtime_uintptr_t tls_base);

/*
 * Get the bootinfo pointer if the process was provided a bootinfo
 * cspace descriptor.
 */
seL4_BootInfo *sel4runtime_bootinfo(void);

/*
 * Get the size in bytes needed to store the thread's TLS.
 */
sel4runtime_size_t sel4runtime_get_tls_size(void);

/*
 * Get the offset used to calculate the thread pointer from the
 * starting address of the thread area.
 */
sel4runtime_size_t sel4runtime_get_tp_offset(void);

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
 * @returns the pointer to the TLS that should be used to call
 *          seL4_TCB_SetTLSBase or NULL on error.
 */
sel4runtime_uintptr_t sel4runtime_write_tls_image(void *tls_memory);

/*
 * Move the TLS for the current thread into a new memory location.
 *
 * Particularly useful for enabling TLS in the initial process where the
 * TLS image exeeded the size of the static TLS region in the runtime.
 *
 * WARNING: This will not prevent re-initialisation of the TLS for the
 * initial thread (even by other threads). This should only be used at
 * the start of a process only if `sel4utils_initial_tls_enabled`
 * returns false.
 *
 * @returns the pointer to the TLS that should be used to call
 *          seL4_TCB_SetTLSBase.
 */
sel4runtime_uintptr_t sel4runtime_move_initial_tls(void *tls_memory);

/*
 * Writes into a thread_local variable of another thread.
 *
 * This assumes that TLS has been initialised for the current thread.
 */
#define sel4runtime_set_tls_variable(thread_pointer, variable, value) ({\
    _Static_assert(\
        sizeof(variable) == sizeof(value), \
        "Set value of same size" \
    ); \
    typeof (variable) typed_value = value; \
    __sel4runtime_write_tls_variable( \
        thread_pointer, \
        (unsigned char *)&(variable), \
        (unsigned char *)&(typed_value), \
        sizeof(typed_value) \
    ); \
})

/*
 * Copies data into the equivalent address in the TLS of another thread.
 *
 * This assumes that TLS has been initialised for the current thread.
 */
int __sel4runtime_write_tls_variable(
    sel4runtime_uintptr_t dest_tls_base,
    unsigned char *local_tls_dest,
    unsigned char *src,
    sel4runtime_size_t bytes
);

/*
 * Callback function type that can be used to exit a process.
 */
typedef void sel4runtime_exit_cb(int code);

/*
 * Set the callback used to exit a process.
 *
 * Returns the current exit function.
 */
sel4runtime_exit_cb *sel4runtime_set_exit(sel4runtime_exit_cb *cb);

/*
 * Callback function type that can be used to exit a process.
 *
 * Should return the exit code to be generated.
 */
typedef int sel4runtime_pre_exit_cb(int code);

/*
 * Set the callback called before destructors are run for a prcess exit..
 *
 * Returns the current pre-exit function.
 */
sel4runtime_pre_exit_cb *sel4runtime_set_pre_exit(sel4runtime_pre_exit_cb *cb);

/*
 * Exit the runtime.
 *
 * This will attempt to suspend the initial thread of the process. Any
 * threading libraries should register a destructor to trigger any
 * threads they create to be suspended. They should neve suspend the
 * current thread.
 */
void sel4runtime_exit(int code);
