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
#include <stddef.h>

#pragma once

// Entry into C program.
int main();

typedef struct {
    int a_type;
    union {
        long a_val;
        void *a_ptr;
        void (*a_fnc)(void);
    } a_un;
} auxv_t;

/*
 * The this triggers the environment to be set up for the runtime before
 * the environment is loaded.
 */
void __sel4runtime_start_main(
    int (*main)(),
    unsigned long argc,
    char **argv,
    char **envp,
    auxv_t auxv[]
);

/*
 * This performs all of the work of loading the execution environment.
 * It mainly operates be loading values out of the environment variables
 * and auxiliary vectors and into variables accessible via interface
 * functions.
 */
void __sel4runtime_load_env(
    char *arg0,
    char **envp,
    auxv_t auxv[]
);
