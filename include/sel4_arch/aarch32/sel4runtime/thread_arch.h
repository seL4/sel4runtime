/*
 * Copyright 2019, Data61
 * Commonwealth Scientific and Industrial Research Organisation (CSIRO)
 * ABN 41 687 119 230.
 *
 * This software may be distributed and modified according to the terms of
 * the BSD 2-Clause license. Note that NO WARRANTY is provided.
 * See "LICENSE_BSD2.txt" for details.
 *
 * @TAG(DATA61_BSD)
 */
#include <autoconf.h>
#include <sel4/arch/constants.h>
#include <sel4runtime/stdint.h>

#if ((__ARM_ARCH_6K__ || __ARM_ARCH_6ZK__) && !__thumb__) \
 || __ARM_ARCH_7A__ || __ARM_ARCH_7R__ || __ARM_ARCH >= 7

static inline sel4runtime_uintptr_t sel4runtime_read_tpidr_el0(void)
{
    sel4runtime_uintptr_t reg;
    __asm__ __volatile__("mrc p15,0,%0,c13,c0,2" : "=r"(reg));
    return reg;
}

static inline void sel4runtime_write_tpidr_el0(sel4runtime_uintptr_t reg)
{
    __asm__ __volatile__("mcr p15,0,%0,c13,c0,2" :: "r"(reg));
}

static inline sel4runtime_uintptr_t sel4runtime_read_tpidrro_el0(void)
{
    sel4runtime_uintptr_t reg;
    __asm__ __volatile__("mrc p15,0,%0,c13,c0,3" : "=r"(reg));
    return reg;
}

/*
 * Set the value of the TLS base for the current thread.
 */
static inline void sel4runtime_set_tls_base(sel4runtime_uintptr_t tls_base)
{
    sel4runtime_write_tpidr_el0(tls_base);
}

#elif defined(CONFIG_KERNEL_GLOBALS_FRAME)

/*
 * In the case of early versions of ARMv6, there are no hardware
 * registers provided for thread-local identifiers. seL4 resolves this
 * by placing the IPC buffer address and thread pointer in a
 * `GlobalsFrame` mapped at the same address in all virtual address
 * spaces. The IPC buffer and thread pointer occupy the first two words
 * in this frame respectively.
 */
static inline sel4runtime_uintptr_t sel4runtime_read_tpidr_el0(void)
{
    void **globals_frame = (void **)seL4_GlobalsFrame;
    return (sel4runtime_uintptr_t)globals_frame[0];
}

static inline sel4runtime_uintptr_t sel4runtime_read_tpidrro_el0(void)
{
    void **globals_frame = (void **)seL4_GlobalsFrame;
    return (sel4runtime_uintptr_t)globals_frame[1];
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
#error "No way to set TLS base provided."
#endif
#endif /* CONFIG_SET_TLS_BASE_SELF */

/*
 * Obtain the value of the TLS base for the current thread.
 */
static inline sel4runtime_uintptr_t sel4runtime_get_tls_base(void)
{
    return sel4runtime_read_tpidr_el0();
}

#define TLS_ABOVE_TP
#define GAP_ABOVE_TP 8
