/*
 * Copyright 2020, Data61
 * Commonwealth Scientific and Industrial Research Organisation (CSIRO)
 * ABN 41 687 119 230.
 *
 * This software may be distributed and modified according to the terms of
 * the BSD 2-Clause license. Note that NO WARRANTY is provided.
 * See "LICENSE_BSD2.txt" for details.
 *
 * @TAG(DATA61_BSD)
 */

/* If no vsyscall implementation is provided, this dummy one is used. */
long __sel4runtime_vsyscall(long sysnum, ...)
{
    return -1;
}

long __attribute__((weak, alias("__sel4runtime_vsyscall"))) sel4_vsyscall(long sysnum, ...);
