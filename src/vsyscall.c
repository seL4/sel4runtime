/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

/* If no vsyscall implementation is provided, this dummy one is used. */
long __sel4runtime_vsyscall(long sysnum, ...)
{
    return -1;
}

long __attribute__((weak, alias("__sel4runtime_vsyscall"))) sel4_vsyscall(long sysnum, ...);
