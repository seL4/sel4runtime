/*
 * Copyright 2019, Data61
 * Commonwealth Scientific and Industrial Research Organisation (CSIRO)
 * ABN 41 687 119 230.
 *
 * This software may be distributed and modified according to the terms of
 * the BSD 2-Clause license. Note that NO WARRANTY is provided.
 * See "LICENSE_BSD2.txt" for details.
 *
 * @TAG(DATA61_BSD)
 */
#pragma once

// Standard auciliary vector values
#define AT_NULL      0
#define PT_LOAD      1
#define PT_DYNAMIC   2
#define AT_PHDR      3
#define AT_PHENT     4
#define AT_PHNUM     5
#define PT_TLS       7
#define PT_NUM       8
#define AT_SYSINFO  32

// seL4-specific auxiliary vector values.
#define AT_SEL4_BOOT_INFO         64
#define AT_SEL4_CSPACE_DESCRIPTOR 65
#define AT_SEL4_VSYSCALL          66
#define AT_SEL4_IPC_BUFFER_PTR    67
#define AT_SEL4_IPC_BUFFER        68
#define AT_SEL4_TCB               69
#define AT_SEL4_CNODE             70
#define AT_SEL4_VSPACE            71
#define AT_SEL4_ASID_POOL         72

typedef struct {
    int a_type;
    union {
        long a_val;
        void *a_ptr;
        void (*a_fnc)(void);
    } a_un;
} auxv_t;

#define ARRAY_LENGTH(a) (sizeof(a) / sizeof(a[0]))
