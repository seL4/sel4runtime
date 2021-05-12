/*
 * Copyright 2019, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include <sel4runtime.h>
#include <sel4runtime/auxv.h>
#include <sel4runtime/mode/elf.h>
#include <sel4runtime/start.h>
#include <sel4/sel4.h>
#include <sel4runtime/gen_config.h>
#include <autoconf.h>

#include "init.h"
#include "util.h"

// Minimum alignment across all platforms.
#define MIN_ALIGN_BYTES 16
#define MIN_ALIGNED __attribute__((aligned (MIN_ALIGN_BYTES)))

// Static TLS for initial thread.
static char static_tls[CONFIG_SEL4RUNTIME_STATIC_TLS] MIN_ALIGNED = {};

// Thread lookup pointers.
typedef struct {
    sel4runtime_uintptr_t tls_base;
} thread_lookup_t;

// The seL4 runtime environment.
static struct {
    char const *process_name;

    // optional bootinfo pointer.
    seL4_BootInfo *bootinfo;

    /*
     * The initial thread object is initially set to a static thread
     * object. It is only used until a TLS is set up for the first
     * thread.
     *
     * Once the TLS has been initialised for the first thread, this is
     * then set to NULL and the thread local reference should be used.
     */
    sel4runtime_uintptr_t initial_thread_tls_base;
    seL4_CPtr initial_thread_tcb;
    seL4_IPCBuffer *initial_thread_ipc_buffer;

    // ELF Headers
    struct {
        sel4runtime_size_t count;
        sel4runtime_size_t size;
        Elf_Phdr *headers;
    } program_header;

    // TLS images
    struct {
        // The location of the initial image in memory.
        void *image;
        // The size of the initial image in memory.
        sel4runtime_size_t image_size;
        // The size needed to store the full TLS.
        sel4runtime_size_t memory_size;
        // The size needed to store the TLS and the thread structure.
        sel4runtime_size_t region_size;
        // Alignment needed for the TLS data.
        sel4runtime_size_t align;
        // Offset of the TLS data from the thread pointer.
        sel4runtime_size_t offset;
    } tls;

    // Argument vector
    char const *const *argv;
    int argc;

    // Auxiliary vector
    auxv_t const *auxv;

    // Environment vector
    char const *const *envp;

    // Exit callbacks
    sel4runtime_exit_cb *exit_cb;
    sel4runtime_pre_exit_cb *pre_exit_cb;
} env = {
    /*
     * Initialise the initial thread as referring to the global thread
     * object.
     */
    .initial_thread_tls_base = (sel4runtime_uintptr_t)SEL4RUNTIME_NULL,
};

static void name_process(char const *name);
static void parse_auxv(auxv_t const auxv[]);
static void parse_phdrs(void);
static void load_tls_data(Elf_Phdr *header);
static void try_init_static_tls(void);
static void copy_tls_data(unsigned char *tls);
static sel4runtime_uintptr_t tls_base_from_tls_region(unsigned char *tls_region);
static unsigned char *tls_from_tls_base(sel4runtime_uintptr_t tls_base);
static unsigned char *tls_from_tls_region(unsigned char *tls_region);
static thread_lookup_t *thread_lookup_from_tls_region(unsigned char *tls_region);
static const sel4runtime_size_t tls_region_size(sel4runtime_size_t mem_size, sel4runtime_size_t align);
static void empty_tls(void);
static int is_initial_thread(void);

char const *sel4runtime_process_name(void)
{
    return env.process_name;
}

char const *const *sel4runtime_argv(void)
{
    return env.argv;
}

int sel4runtime_argc(void)
{
    return env.argc;
}

char const *const *sel4runtime_envp(void)
{
    return env.envp;
}

auxv_t const *sel4runtime_auxv(void)
{
    return env.auxv;
}

seL4_BootInfo *sel4runtime_bootinfo(void)
{
    return env.bootinfo;
}

sel4runtime_size_t sel4runtime_get_tls_size(void)
{
    return env.tls.region_size;
}

int sel4runtime_initial_tls_enabled(void)
{
    /*
     * If the TLS for the initial process has been activated, the thread
     * object in the TLS will be used rather than the static thread
     * object.
     */
    return env.initial_thread_tls_base != (sel4runtime_uintptr_t)SEL4RUNTIME_NULL;
}

sel4runtime_uintptr_t sel4runtime_write_tls_image(void *tls_memory)
{
    if (tls_memory == SEL4RUNTIME_NULL) {
        return (sel4runtime_uintptr_t)SEL4RUNTIME_NULL;
    }

    copy_tls_data(tls_memory);

    return tls_base_from_tls_region(tls_memory);
}

sel4runtime_uintptr_t sel4runtime_move_initial_tls(void *tls_memory)
{
    if (tls_memory == SEL4RUNTIME_NULL) {
        return (sel4runtime_uintptr_t)SEL4RUNTIME_NULL;
    }

    sel4runtime_uintptr_t tls_base = sel4runtime_write_tls_image(tls_memory);
    if (tls_base == (sel4runtime_uintptr_t)SEL4RUNTIME_NULL) {
        return (sel4runtime_uintptr_t)SEL4RUNTIME_NULL;
    }

    sel4runtime_set_tls_base(tls_base);

    if (env.initial_thread_ipc_buffer != SEL4RUNTIME_NULL) {
        __sel4_ipc_buffer = env.initial_thread_ipc_buffer;
    }

    env.initial_thread_tls_base = tls_base;

#if defined(CONFIG_DEBUG_BUILD)
    if (env.initial_thread_tcb && env.initial_thread_ipc_buffer && env.process_name) {
        // The thread can only be named after the TLS is initialised
        // and if an IPC buffer is present.
        seL4_DebugNameThread(env.initial_thread_tcb, env.process_name);
    }
#endif

    return env.initial_thread_tls_base;
}

sel4runtime_exit_cb *sel4runtime_set_exit(sel4runtime_exit_cb *cb)
{
    sel4runtime_exit_cb *old = env.exit_cb;
    env.exit_cb = cb;
    return old;
}

sel4runtime_pre_exit_cb *sel4runtime_set_pre_exit(sel4runtime_pre_exit_cb *cb)
{
    sel4runtime_pre_exit_cb *old = env.pre_exit_cb;
    env.pre_exit_cb = cb;
    return old;
}

void sel4runtime_exit(int code)
{
    if (env.pre_exit_cb != SEL4RUNTIME_NULL) {
        code = env.pre_exit_cb(code);
    }

    __sel4runtime_run_destructors();

    /* If the exit is never set this will try and call a NULL function
     * pointer which should result in a fault. This is as good a way as
     * any to exit the process if we don't know anything better about
     * the environment. */
    env.exit_cb(code);
}

int __sel4runtime_write_tls_variable(
    sel4runtime_uintptr_t dest_tls_base,
    unsigned char *local_tls_dest,
    unsigned char *src,
    sel4runtime_size_t bytes
)
{
    sel4runtime_uintptr_t local_tls_base = sel4runtime_get_tls_base();
    unsigned char *local_tls = tls_from_tls_base(local_tls_base);
    sel4runtime_size_t offset = local_tls_dest - local_tls;
    sel4runtime_size_t tls_size = env.tls.memory_size;

    // Write must not go past end of TLS.
    if (offset > tls_size || offset + bytes > tls_size) {
        return -1;
    }

    unsigned char *dest_tls = tls_from_tls_base(dest_tls_base);
    unsigned char *dest_addr = dest_tls + offset;

    __sel4runtime_memcpy(dest_addr, src, bytes);

    return 0;
}

void __sel4runtime_load_env(
    int argc,
    char const *const *argv,
    char const *const *envp,
    auxv_t const auxv[]
)
{
    empty_tls();
    parse_auxv(auxv);
    parse_phdrs();
    if (argc > 0) {
        name_process(argv[0]);
    }
    try_init_static_tls();

    env.argc = argc;
    env.argv = argv;
    env.auxv = auxv;
    env.envp = envp;

    __sel4runtime_run_constructors();
}

static void name_process(char const *name)
{
    env.process_name = name;

    while (name && *name != '\0') {
        if (*name == '/') {
            env.process_name = name + 1;
        }

        name++;
    }
}

static void parse_auxv(auxv_t const auxv[])
{
    for (int i = 0; auxv[i].a_type != AT_NULL; i++) {
        switch (auxv[i].a_type) {
        case AT_PHENT: {
            env.program_header.size = auxv[i].a_un.a_val;
            break;
        }
        case AT_PHNUM: {
            env.program_header.count = auxv[i].a_un.a_val;
            break;
        }
        case AT_PHDR: {
            env.program_header.headers = auxv[i].a_un.a_ptr;
            break;
        }
        case AT_SEL4_BOOT_INFO: {
            seL4_BootInfo *bootinfo = auxv[i].a_un.a_ptr;
            if (bootinfo == SEL4RUNTIME_NULL) {
                break;
            }
            env.bootinfo = bootinfo;
            env.initial_thread_ipc_buffer = bootinfo->ipcBuffer;
            env.initial_thread_tcb = seL4_CapInitThreadTCB;
            break;
        }
        case AT_SEL4_IPC_BUFFER_PTR: {
            env.initial_thread_ipc_buffer = auxv[i].a_un.a_ptr;
            break;
        }
        case AT_SEL4_TCB: {
            env.initial_thread_tcb = auxv[i].a_un.a_val;
            break;
        }
        default:
            break;
        }
    }
}

static void parse_phdrs(void)
{
    for (sel4runtime_size_t h = 0; h < env.program_header.count; h++) {
        Elf_Phdr *header = &env.program_header.headers[h];
        switch (header->p_type) {
        case PT_TLS:
            load_tls_data(header);
            break;
        default:
            break;
        }
    }
}

static void load_tls_data(Elf_Phdr *header)
{
    env.tls.image = (void *) header->p_vaddr;
    if (header->p_align > MIN_ALIGN_BYTES) {
        env.tls.align = header->p_align;
    } else {
        env.tls.align = MIN_ALIGN_BYTES;
    }
    env.tls.image_size = header->p_filesz;
    env.tls.memory_size = ROUND_UP(header->p_memsz, header->p_align);
    env.tls.region_size = tls_region_size(
                              env.tls.memory_size,
                              env.tls.align
                          );
}

static void try_init_static_tls(void)
{
    if (env.tls.region_size <= sizeof(static_tls)) {
        sel4runtime_move_initial_tls(static_tls);
    }
}

static void copy_tls_data(unsigned char *tls_region)
{
    unsigned char *tls = tls_from_tls_region(tls_region);
    __sel4runtime_memcpy(tls, env.tls.image, env.tls.image_size);
    unsigned char *tbss = &tls[env.tls.image_size];
    __sel4runtime_memset(tbss, 0, env.tls.memory_size - env.tls.image_size);

    thread_lookup_t *lookup = thread_lookup_from_tls_region(tls_region);
    if (lookup != SEL4RUNTIME_NULL) {
        lookup->tls_base = tls_base_from_tls_region(tls_region);
    }
}

static sel4runtime_uintptr_t tls_base_from_tls_region(unsigned char *tls_region)
{
    sel4runtime_uintptr_t tls_base = (sel4runtime_uintptr_t)tls_region;
#if !defined(TLS_ABOVE_TP)
    tls_base += env.tls.memory_size;
#endif
    return ROUND_UP(tls_base, env.tls.align);
}

static unsigned char *tls_from_tls_base(sel4runtime_uintptr_t tls_base)
{
    sel4runtime_uintptr_t tls_addr = tls_base;
#if !defined(TLS_ABOVE_TP)
    tls_addr -= env.tls.memory_size;
#endif
#if defined(GAP_ABOVE_TP)
    tls_addr +=  GAP_ABOVE_TP;
#endif
    return (unsigned char *)tls_addr;
}

static unsigned char *tls_from_tls_region(unsigned char *tls_region)
{
    return tls_from_tls_base(tls_base_from_tls_region(tls_region));
}

static thread_lookup_t *thread_lookup_from_tls_region(
    unsigned char *tls_region
)
{
#if !defined(TLS_ABOVE_TP)
    return (thread_lookup_t *)tls_base_from_tls_region(tls_region);
#else
    return SEL4RUNTIME_NULL;
#endif
}

static const sel4runtime_size_t tls_region_size(sel4runtime_size_t mem_size, sel4runtime_size_t align)
{
    return align
           + ROUND_UP(sizeof(thread_lookup_t), align)
#if defined(GAP_ABOVE_TP)
           + ROUND_UP(GAP_ABOVE_TP, align)
#endif
           + ROUND_UP(mem_size, align);
}

static void empty_tls(void)
{
    env.tls.image = SEL4RUNTIME_NULL;
    env.tls.align = MIN_ALIGN_BYTES;
    env.tls.image_size = 0;
    env.tls.memory_size = 0;
    env.tls.region_size = tls_region_size(
                              env.tls.memory_size,
                              env.tls.align
                          );
}

/*
 * Check if the executing thread is the inital thread of the process.
 *
 * This will optimistically assume that the current thread is the
 * initial thread of no thread ever had TLS configured.
 */
static int is_initial_thread(void)
{
    return env.initial_thread_tls_base == (sel4runtime_uintptr_t)SEL4RUNTIME_NULL
           || sel4runtime_get_tls_base() == env.initial_thread_tls_base;
}
