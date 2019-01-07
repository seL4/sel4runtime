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
#include <assert.h>
#include <stdbool.h>
#include <string.h>
#include <sel4runtime.h>
#include <sel4runtime/auxv.h>
#include <sel4/sel4.h>

#include <mode/elf_helper.h>

#include "start.h"
#include "thread.h"
#include "init.h"
#include "util.h"

// Minimum alignment across all platforms.
#define WORD_SIZE (sizeof(seL4_Word))
#define WORD_ALIGNED __attribute__((aligned (WORD_SIZE)))

// Global vsyscall handler.
size_t __sysinfo;

static thread_t WORD_ALIGNED __initial_thread;

// Static TLS for initial thread.
char WORD_ALIGNED static_tls[CONFIG_SEL4RUNTIME_STATIC_TLS];

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
    thread_t *initial_thread;
    bool initial_thread_tls_enabled;
    seL4_CPtr initial_thread_tcb;
    seL4_IPCBuffer *initial_thread_ipc_buffer;

    // ELF Headers
    struct {
        size_t count;
        size_t size;
        Elf_Phdr *headers;
    } program_header;

    // TLS images
    struct {
        // The location of the initial image in memory.
        void *image;
        // The size of the initial image in memory.
        size_t image_size;
        // The size needed to store the full TLS.
        size_t memory_size;
        // The size needed to store the TLS and the thread structure.
        size_t region_size;
        // Alignment needed for the TLS data.
        size_t align;
        // Offset of the TLS data from the thread pointer.
        size_t offset;
    } tls;

    // Auxiliary vector
    auxv_t const *auxv;

    // Environment vector
    char const * const *envp;
} env = {
    /*
     * Initialise the initial thread as referring to the global thread
     * object.
     */
    .initial_thread = &__initial_thread,
    .initial_thread_tls_enabled = false,
};

static void name_process(char const *name);
static void parse_auxv(auxv_t const auxv[]);
static void parse_phdrs(void);
static void load_tls_data(Elf_Phdr *header);
static void try_init_static_tls(void);
static void copy_tls_data(thread_t *thread);
static thread_t *thread_from_tls_region(void *tls_region);
static char *tls_from_tls_region(void *tls_region);
static void *tls_base_addr(thread_t *thread);
static const size_t tls_region_size(size_t mem_size, size_t align);
static void empty_tls(void);
static bool is_initial_thread(void);
static thread_t *thread_from_base(void *tls_base);

char const *sel4runtime_process_name(void) {
    return env.process_name;
}

const seL4_BootInfo *sel4runtime_bootinfo(void) {
    return env.bootinfo;
}

void *sel4runtime_tls_base_addr(void) {
    return tls_base_addr(__sel4runtime_thread_self());
}

size_t sel4runtime_get_tls_size(void) {
    return env.tls.region_size;
}

size_t sel4runtime_get_tp_offset(void) {
    return (size_t) tls_base_addr(thread_from_tls_region((void *) 0x0));
}

int sel4runtime_initial_tls_enabled(void) {
    /*
     * If the TLS for the initial process has been activated, the thread
     * object in the TLS will be used rather than the static thread
     * object.
     */
    return env.initial_thread_tls_enabled;
}

void *sel4runtime_write_tls_image(void *tls_memory) {
    if (!sel4runtime_initial_tls_enabled() || tls_memory == NULL) {
        return NULL;
    }

    thread_t *thread = thread_from_tls_region(tls_memory);
    thread->self = thread;
    thread->tls = tls_from_tls_region(tls_memory);
    thread->tls_region = tls_memory;
    thread->errno = 0;

    copy_tls_data(thread);

    return tls_base_addr(thread);
}

void *sel4runtime_move_initial_tls(void *tls_memory) {
    if (
        tls_memory == NULL ||
        env.initial_thread_tcb == seL4_CapNull
    ) {
        return NULL;
    }

    thread_t *thread = thread_from_tls_region(tls_memory);
    *thread = *env.initial_thread;
    thread->self = thread;
    thread->tls = tls_from_tls_region(tls_memory);
    thread->tls_region = tls_memory;

    copy_tls_data(thread);

    seL4_Error err = seL4_TCB_SetTLSBase(
        env.initial_thread_tcb,
        (uintptr_t)tls_base_addr(thread)
    );
    if (err != seL4_NoError) {
        return NULL;
    }

    __sel4_ipc_buffer = env.initial_thread_ipc_buffer;

    env.initial_thread_tls_enabled = true;

    // The thread can only be named after the TLS is initialised.
#if defined(CONFIG_DEBUG_BUILD)
    seL4_DebugNameThread(env.initial_thread_tcb, env.process_name);
#endif

    return tls_base_addr(thread);
}

void sel4runtime_exit(int code) {
    if (!is_initial_thread()) {
        seL4_TCB_Suspend(env.initial_thread_tcb);
    }

    __sel4runtime_run_destructors();

    /* Suspend the process */
    while (true) {
        if (is_initial_thread()) {
            seL4_TCB_Suspend(env.initial_thread_tcb);
        }
        seL4_Yield();
    }
}

int sel4runtime_write_tls_variable(
    void *dest_tls_base,
    void *local_tls_dest,
    void *src,
    size_t bytes
) {
    thread_t *local_thread = __sel4runtime_thread_self();
    size_t offset = (uintptr_t)local_tls_dest - (uintptr_t)local_thread->tls_region;
    size_t tls_size = sel4runtime_get_tls_size();

    // Write must not go past end of TLS.
    if (offset > tls_size || offset + bytes > tls_size) {
        return -1;
    }

    thread_t *dest_thread = thread_from_base(dest_tls_base);
    uintptr_t dest_addr = (uintptr_t)dest_thread->tls_region + offset;

    __sel4runtime_memcpy((void *)dest_addr, src, bytes);

    return 0;
}

thread_t *__sel4runtime_thread_self(void) {
    if (!sel4runtime_initial_tls_enabled()) {
        return env.initial_thread;
    } else {
        void *tls_base = (void *)__sel4runtime_thread_pointer();
        return thread_from_base(tls_base);
    }
}

void __sel4runtime_load_env(
    char const *arg0,
    char const * const *envp,
    auxv_t const auxv[]
) {
    __sel4runtime_run_constructors();
    empty_tls();
    parse_auxv(auxv);
    parse_phdrs();
    name_process(arg0);
    try_init_static_tls();

    env.auxv = auxv;
    env.envp = envp;
}

static void name_process(char const *name) {
    env.process_name = name;

    while (*name != '\0') {
        if (*name == '/') {
            env.process_name = name + 1;
        }

        name++;
    }
}

static void parse_auxv(auxv_t const auxv[]) {
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
        case AT_SYSINFO: {
            __sysinfo = (size_t)(auxv[i].a_un.a_ptr);
            break;
        }
        case AT_SEL4_BOOT_INFO: {
            seL4_BootInfo *bootinfo = auxv[i].a_un.a_ptr;
            if (bootinfo == NULL) break;
            env.bootinfo = bootinfo;
            thread_t *thread = __sel4runtime_thread_self();
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

static void parse_phdrs(void) {
    for (size_t h = 0; h < env.program_header.count; h++) {
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

static void load_tls_data(Elf_Phdr *header) {
    env.tls.image = (void *) header->p_vaddr;
    if (header->p_align >= WORD_SIZE) {
        env.tls.align = header->p_align;
    } else {
        env.tls.align = WORD_SIZE;
    }
    env.tls.image_size = header->p_filesz;
    env.tls.memory_size = header->p_memsz;
    env.tls.region_size = tls_region_size(
        env.tls.memory_size,
        env.tls.align
    );
}

static void try_init_static_tls(void) {
    if (
        env.initial_thread_tcb != seL4_CapNull &&
        env.tls.region_size <= sizeof(static_tls)
    ) {
        sel4runtime_move_initial_tls(static_tls);
    }
}

static void copy_tls_data(thread_t *thread) {
    __sel4runtime_memcpy(thread->tls, env.tls.image, env.tls.image_size);
    char *tbss = &((char *)thread->tls)[env.tls.image_size];
    __sel4runtime_memset(tbss, 0, env.tls.memory_size - env.tls.image_size);
}

static thread_t *thread_from_tls_region(void *tls_region) {
    uintptr_t tls_addr = ROUND_UP((uintptr_t)tls_region, env.tls.align);
#if !defined(TLS_ABOVE_TP)
    tls_addr += ROUND_UP(env.tls.memory_size, env.tls.align);
#endif
    return (thread_t *)tls_addr;
}

static char *tls_from_tls_region(void *tls_region) {
    uintptr_t tls_addr = (uintptr_t)thread_from_tls_region(tls_region);
#if defined(TLS_ABOVE_TP)
    tls_addr += ROUND_UP(sizeof(thread_t), env.tls.align);
#if defined(GAP_ABOVE_TP)
    tls_addr +=  GAP_ABOVE_TP;
#endif
#else
    tls_addr -= ROUND_UP(env.tls.memory_size, env.tls.align);
#endif
    return (char *)tls_addr;
}

static void *tls_base_addr(thread_t *thread) {
    uintptr_t tls_base = (uintptr_t)thread;
#if defined(TLS_ABOVE_TP)
    tls_base += ROUND_UP(sizeof(thread_t), env.tls.align);
#endif
    return (void *)tls_base;
}

static const size_t tls_region_size(size_t mem_size, size_t align) {
    return align
        + ROUND_UP(sizeof (thread_t), align)
#if defined(GAP_ABOVE_TP)
        + GAP_ABOVE_TP
#endif
        + ROUND_UP(mem_size, align);
}

static void empty_tls(void) {
    env.tls.image = NULL;
    env.tls.align = WORD_SIZE;
    env.tls.image_size = 0;
    env.tls.memory_size = 0;
    env.tls.region_size = tls_region_size(
        env.tls.memory_size,
        env.tls.align
    );
}

/*
 * Check if the executing thread is the inital thread of the process.
 */
static bool is_initial_thread(void) {
    void *thread_tls = __sel4runtime_thread_self()->tls;
    return thread_tls == env.initial_thread->tls;
}

/*
 * Get the hread object from the TLS base of a thread.
 */
static thread_t *thread_from_base(void *tls_base) {
    uintptr_t thread = (uintptr_t)tls_base;
#if defined(TLS_ABOVE_TP)
    thread -= ROUND_UP(sizeof(thread_t), env.tls.align);
#endif
    return (thread_t *)thread;
}
