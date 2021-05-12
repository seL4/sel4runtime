/*
 * Copyright 2019, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include <sel4/sel4.h>
#include <sel4runtime.h>
#include <sel4runtime/start.h>

void __sel4runtime_start_main(
    int (*main)(),
    unsigned long argc,
    char const *const *argv,
    char const *const *envp,
    auxv_t const auxv[]
)
{
    __sel4runtime_load_env(argc, argv, envp, auxv);

    sel4runtime_exit(main(argc, argv, envp));
}
