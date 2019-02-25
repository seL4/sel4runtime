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

#include <sel4/arch/constants.h>
#include <stdint.h>

#if ((__ARM_ARCH_6K__ || __ARM_ARCH_6ZK__) && !__thumb__) \
 || __ARM_ARCH_7A__ || __ARM_ARCH_7R__ || __ARM_ARCH >= 7

static inline uintptr_t __sel4runtime_read_tpidr_el0(void) {
    uintptr_t reg;
    __asm__ __volatile__ ( "mrc p15,0,%0,c13,c0,2" : "=r"(reg) );
    return reg;
}

static inline uintptr_t __sel4runtime_write_tpidr_el0(uintptr_t reg) {
    __asm__ __volatile__ ( "mcr p15,0,%0,c13,c0,2" : "=r"(reg) );
    return;
}

static inline uintptr_t __sel4runtime_read_tpidrro_el0(void) {
    uintptr_t reg;
    __asm__ __volatile__ ( "mrc p15,0,%0,c13,c0,3" : "=r"(reg) );
    return reg;
}

#elif defined(CONFIG_ARCH_ARM_V6)

/*
 * In the case of early versions of ARMv6, there are no hardware
 * registers provided for thread-local identifiers. seL4 resolves this
 * by placing the IPC buffer address and thread pointer in a
 * `GlobalsFrame` mapped at the same address in all virtual address
 * spaces. The IPC buffer and thread pointer occupy the first two words
 * in this frame respectively.
 */
static inline uintptr_t __sel4runtime_read_tpidr_el0(void) {
    void **globals_frame = (void **)seL4_GlobalsFrame;
    return (uintptr_t)globals_frame[0];
}

static inline uintptr_t __sel4runtime_read_tpidrro_el0(void) {
    void **globals_frame = (void **)seL4_GlobalsFrame;
    return (uintptr_t)globals_frame[1];
}

#else

#error "ARM architectures below ARMv6 are unsupported"

#endif

/*
 * Obtain the value of the TLS base for the current thread.
 */
static inline uintptr_t __sel4runtime_thread_pointer(void) {
    return __sel4runtime_read_tpidr_el0();
}

#define TLS_ABOVE_TP
#define GAP_ABOVE_TP 8
