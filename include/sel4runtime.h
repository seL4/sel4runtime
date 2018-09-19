/*
 * The seL4 run-time interface.
 *
 * This provides an interface to the values managed by sel4runtime.
 */
#include <stddef.h>
#include <sel4/sel4.h>

#pragma once

/*
 * Get the name of the process.
 *
 * Returns the basename of the first argument in argv.
 */
char const *sel4runtime_process_name(void);

/*
 * Get the bootinfo pointer if the process was provided a bootinfo
 * cspace descriptor.
 */
const seL4_BootInfo *sel4runtime_bootinfo(void);
