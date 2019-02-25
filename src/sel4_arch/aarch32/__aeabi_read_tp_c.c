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
#include <autoconf.h>
#include <sel4/sel4.h>
#include <thread_arch.h>

__attribute__((__visibility__("hidden")))
void *__aeabi_read_tp_c(void) {
    return (void *)__sel4runtime_thread_pointer();
}
