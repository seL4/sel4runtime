/*
 * Copyright 2019, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include <sel4/sel4.h>
#include <sel4runtime/start.h>
#include <sel4runtime/stdint.h>
#include <sel4runtime/stddef.h>

/*
 * As this file is only included when we are running a root server,
 * these symbols must exist and be provided for this file to function
 * properly.
 *
 * This will generate a link time error if this function is used outside
 * of a root server.
 */

extern unsigned int _tdata_start[];
extern unsigned int _tdata_end[];
extern unsigned int _tbss_start[];
extern unsigned int _tbss_end[];

long sel4_vsyscall(long sysnum, ...);

/*
 * The entrypoint into a root task is somewhat different to the
 * entrypoint into a regular process. The kernel does not provide a
 * stack to the root task nor does it conform to System-V ABI; instead
 * it simply starts execution at the entrypoint with the first argument
 * being the pointer to the seL4_BootInfo.
 *
 * This is invoked by _sel4_start, which simply sets up a static stack
 * and passes the argument to us.
 */
void __sel4_start_root(seL4_BootInfo *boot_info)
{
    sel4runtime_uintptr_t tdata_start = (sel4runtime_uintptr_t) &_tdata_start[0];
    sel4runtime_uintptr_t tdata_end = (sel4runtime_uintptr_t) &_tdata_end[0];
    sel4runtime_uintptr_t tbss_start = (sel4runtime_uintptr_t) &_tbss_start[0];
    sel4runtime_uintptr_t tbss_end = (sel4runtime_uintptr_t) &_tbss_end[0];

    /* Unlike ld.bfd, lld assigns 0 address to empty .tdata output
     * sections and its linker variables. This will make
     * _tdata_start and _tdata_end zero.
     * Assuming _tdata_end in this code to be adjacent to _tbss_start would be
     * wrong and will trigger address faults as the TLS'
     * region p_memsz value will be wrong.
     * Assigning empty tbss_start to tdata[_start|_end] will
     * follow ld.bfd and results in the proper 0 p_memsz.
     * The .tdata section is empty and hence, the linker script variables are zero
     */
    if (tdata_start == 0) {
        tdata_start = tbss_start;
        tdata_end = tbss_start;
    }

    Elf_Phdr tls_header = {
        .p_type   = PT_TLS,
        .p_offset = 0,
        .p_vaddr  = (Elf_Addr) tdata_start,
        .p_paddr  = 0,
        .p_filesz = tdata_end - tdata_start,
        .p_memsz  = tbss_end - tdata_start,
        .p_align = sizeof(seL4_Word),
    };

    struct {
        char const *const argv[2];
        char const *const envp[2];
        auxv_t auxv[8];
    } info = {
        .argv = {
            "rootserver",
            SEL4RUNTIME_NULL,
        },
        .envp = {
            "seL4=1",
            SEL4RUNTIME_NULL,
        },
        .auxv = {
            {
                .a_type = AT_PHENT,
                .a_un.a_val = sizeof(Elf32_Phdr),
            }, {
                .a_type = AT_PHNUM,
                .a_un.a_val = 1,
            }, {
                .a_type = AT_PHDR,
                .a_un.a_ptr = &tls_header,
            }, {
                .a_type = AT_SYSINFO,
                .a_un.a_ptr = &sel4_vsyscall,
            }, {
                .a_type = AT_SEL4_BOOT_INFO,
                .a_un.a_ptr = boot_info,
            }, {
                .a_type = AT_SEL4_TCB,
                .a_un.a_val = seL4_CapInitThreadTCB,
            }, {
                .a_type = AT_PAGESZ,
                .a_un.a_val = 0x1000,
            }, {
                // Null terminating entry
                .a_type = AT_NULL,
                .a_un.a_val = 0
            },
        },
    };

    __sel4runtime_start_main(main, ARRAY_LENGTH(info.argv) - 1, info.argv, info.envp, info.auxv);
}
