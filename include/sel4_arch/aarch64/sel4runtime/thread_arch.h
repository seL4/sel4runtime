/*
 * Copyright 2019, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include <sel4runtime/stdint.h>

/*
 * Obtain the value of the TLS base for the current thread.
 */
static inline sel4runtime_uintptr_t sel4runtime_read_tpidr_el0(void)
{
    sel4runtime_uintptr_t reg;
    __asm__ __volatile__("mrs %0,tpidr_el0" : "=r"(reg));
    return reg;
}

static inline void sel4runtime_write_tpidr_el0(sel4runtime_uintptr_t reg)
{
    __asm__ __volatile__("msr tpidr_el0,%0" :: "r"(reg));
}

static inline sel4runtime_uintptr_t sel4runtime_read_tpidrro_el0(void)
{
    sel4runtime_uintptr_t reg;
    __asm__ __volatile__("mrs %0,tpidrro_el0" : "=r"(reg));
    return reg;
}

/*
 * Obtain the value of the TLS base for the current thread.
 */
static inline sel4runtime_uintptr_t sel4runtime_get_tls_base(void)
{
    return sel4runtime_read_tpidr_el0();
}

/*
 * Set the value of the TLS base for the current thread.
 */
static inline void sel4runtime_set_tls_base(sel4runtime_uintptr_t tls_base)
{
    sel4runtime_write_tpidr_el0(tls_base);
}

#define TLS_ABOVE_TP
#define GAP_ABOVE_TP 16
