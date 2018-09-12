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
 * Descriptors of the process's cspace.
 */
typedef struct {
    enum sel4runtime_cspace_tag {
        SEL4RUNTIME_CSPACE_NONE,
        SEL4RUNTIME_CSPACE_BOOTINFO,
    } tag;
    union {
        seL4_BootInfo *bootinfo;
    } cspace;
} sel4runtime_cspace_t;

/*
 * Get the cspace descriptor for this process.
 */
const sel4runtime_cspace_t *sel4runtime_cspace_descriptor(void);

/*
 * Determine the type of cspace descriptor provided to this process.
 */
static inline enum sel4runtime_cspace_tag sel4runtime_cspace_type(void) {
    return sel4runtime_cspace_descriptor()->tag;
}

/*
 * Get the bootinfo pointer if the process was provided a bootinfo
 * cspace descriptor.
 */
static inline const seL4_BootInfo *sel4runtime_bootinfo(void) {
    sel4runtime_cspace_t const *descriptor
        = sel4runtime_cspace_descriptor();
    if (descriptor->tag == SEL4RUNTIME_CSPACE_BOOTINFO) {
        return descriptor->cspace.bootinfo;
    } else {
        return NULL;
    }
}
