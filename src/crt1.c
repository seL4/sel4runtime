/*
 * Copyright 2019, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include <sel4runtime.h>
#include <sel4runtime/start.h>

/*
 * This function is simply passed a pointer to the inital stack from the
 * C runtime entrypoint.
 *
 * The stack has the following structure:
 *
 * * argument count,
 * * array of argument pointers,
 * * an empty string,
 * * array of environment pointers,
 * * a null terminator,
 * * array of auxiliary vector entries,
 * * an 'zero' auxiliary vector, then
 * * unspecified data.
 */
void __sel4_start_c(void const *stack)
{
    // First word on the stack is argument count.
    unsigned long argc = *((unsigned long const *) stack);

    // Second word on the stack is the start of the argument vector.
    char const *const *argv = &((char const * const *) stack)[1];

    // The environment pointer vector follows after the argv.
    char const *const *envp = &argv[argc + 1];
    int envc = 0;
    while (envp[envc] != SEL4RUNTIME_NULL) {
        envc++;
    }


    // The auxiliary vector follows the environment pointer vector.
    auxv_t const *auxv = (void const *)(&envp[envc + 1]);

    __sel4runtime_start_main(main, argc, argv, envp, auxv);
}
