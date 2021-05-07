/*
 * Copyright 2019, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include <sel4/sel4.h>
#include <sel4runtime/thread_arch.h>

__attribute__((__visibility__("hidden")))
void *__aeabi_read_tp_c(void)
{
    return (void *)sel4runtime_get_tls_base();
}
