/*
 * Copyright 2019, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include <autoconf.h>
#include <sel4runtime/stdint.h>

/*
 * Obtain the value of the TLS base for the current thread.
 */
static inline sel4runtime_uintptr_t sel4runtime_get_tls_base(void)
{
    sel4runtime_uintptr_t tp;
    __asm__ __volatile__("movl %%gs:0,%0" : "=r"(tp));
    return tp;
}

#ifdef CONFIG_SET_TLS_BASE_SELF
/*
 * Set the value of the TLS base for the current thread.
 */
static inline void sel4runtime_set_tls_base(sel4runtime_uintptr_t tls_base)
{
    seL4_SetTLSBase(tls_base);
}
#else
#error "Set TLS for ia32 not implemented"
#endif /* CONFIG_SET_TLS_BASE_SELF */
