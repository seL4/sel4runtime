/*
 * Copyright 2019, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include <autoconf.h>
#include <sel4runtime/stdint.h>

#ifdef CONFIG_FSGSBASE_INST
static inline sel4runtime_uintptr_t sel4runtime_read_fs_base(void)
{
    sel4runtime_uintptr_t reg;
    __asm__ __volatile__("rdfsbase %0" : "=r"(reg));
    return reg;
}

static inline void sel4runtime_write_fs_base(sel4runtime_uintptr_t reg)
{
    __asm__ __volatile__("wrfsbase %0" :: "r"(reg));
}

static inline sel4runtime_uintptr_t sel4runtime_read_gs_base(void)
{
    sel4runtime_uintptr_t reg;
    __asm__ __volatile__("rdgsbase %0" : "=r"(reg));
    return reg;
}

static inline void sel4runtime_write_gs_base(sel4runtime_uintptr_t reg)
{
    __asm__ __volatile__("wrgsbase %0" :: "r"(reg));
}

/*
 * Obtain the value of the TLS base for the current thread.
 */
static inline sel4runtime_uintptr_t sel4runtime_get_tls_base(void)
{
    return sel4runtime_read_fs_base();
}

/*
 * Set the value of the TLS base for the current thread.
 */
static inline void sel4runtime_set_tls_base(sel4runtime_uintptr_t tls_base)
{
    sel4runtime_write_fs_base(tls_base);
}

#else

/*
 * Obtain the value of the TLS base for the current thread.
 */
static inline sel4runtime_uintptr_t sel4runtime_get_tls_base(void)
{
    sel4runtime_uintptr_t tp;
    __asm__ __volatile__("mov %%fs:0,%0" : "=r"(tp));
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
#error "Set TLS for x86_64 w/o FSGSBASE_INST not implemented"
#endif /* CONFIG_SET_TLS_BASE_SELF */

#endif

