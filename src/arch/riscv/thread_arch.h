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

#include <stdint.h>

/*
 * Obtain the value of the TLS base for the current thread.
 */
static inline uintptr_t __sel4runtime_thread_pointer() {
#ifdef __clang__
    uintptr_t tp;
    __asm__ __volatile__ ("or %0, tp, x0" : "=r" (tp) );
#else
    register uintptr_t tp __asm__("tp");
#endif
    return tp;
}

#define TLS_ABOVE_TP
