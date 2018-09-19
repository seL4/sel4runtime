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
#include <string.h>
#include <sel4runtime.h>
#include <sel4/sel4.h>
#include <thread_arch.h>
#include <utils/arith.h>

#include "auxv.h"
#include "start.h"
#include "thread.h"


static thread_t __attribute__((aligned (16))) __initial_thread;

// Static TLS for initial thread.
char __attribute__((aligned (16))) static_tls[CONFIG_SEL4RUNTIME_STATIC_TLS];

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

    // ELF Headers
    struct {
        size_t count;
        size_t size;
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
};

static void name_process(char const *name);
static void parse_auxv(auxv_t const auxv[]);
static void parse_phdr(Elf_Phdr* header);
static void load_tls_data(Elf_Phdr *header);
static void try_init_static_tls(void);
static thread_t *thread_from_tls_region(void *tls_region);
static char *tls_from_tls_region(void *tls_region);

char const *sel4runtime_process_name(void) {
    return env.process_name;
}

const seL4_BootInfo *sel4runtime_bootinfo(void) {
    return env.bootinfo;
}

seL4_IPCBuffer *sel4runtime_ipc_buffer() {
    return __sel4runtime_thread_self()->ipc_buffer;
}
seL4_IPCBuffer *seL4_GetIPCBuffer(void) {
    return __sel4runtime_thread_self()->ipc_buffer;
}

seL4_CPtr sel4runtime_thread_ipc_buffer() {
    return __sel4runtime_thread_self()->ipc_buffer_page;
}

seL4_CPtr sel4runtime_thread_tcb() {
    return __sel4runtime_thread_self()->tcb;
}

seL4_CPtr sel4runtime_thread_cnode() {
    return __sel4runtime_thread_self()->cnode;
}

seL4_CPtr sel4runtime_thread_vspace() {
    return __sel4runtime_thread_self()->vspace;
}

seL4_CPtr sel4runtime_thread_asid_pool() {
    return __sel4runtime_thread_self()->asid_pool;
}

size_t sel4runtime_get_tls_size(void) {
    return env.tls.region_size;
}

int sel4runtime_initial_tls_enabled(void) {
    /*
     * If the TLS for the initial process has been activated, the thread
     * object in the TLS will be used rather than the static thread
     * object.
     */
    return env.initial_thread == NULL;
}

void *sel4runtime_write_tls_image(
    void *tls_memory,
    seL4_CPtr tcb,
    seL4_IPCBuffer *ipc_buffer,
    seL4_CPtr ipc_buffer_page
) {
    return sel4runtime_write_tls_image_extended(
        tls_memory,
        tcb,
        ipc_buffer,
        ipc_buffer_page,
        sel4runtime_thread_cnode(),
        sel4runtime_thread_vspace(),
        sel4runtime_thread_asid_pool()
    );
}

void *sel4runtime_write_tls_image_extended(
    void *tls_memory,
    seL4_CPtr tcb,
    seL4_IPCBuffer *ipc_buffer,
    seL4_CPtr ipc_buffer_page,
    seL4_CPtr cnode,
    seL4_CPtr vspace,
    seL4_CPtr asid_pool
) {
    if (!sel4runtime_initial_tls_enabled() || tls_memory == NULL) {
        return NULL;
    }

    thread_t *thread = thread_from_tls_region(tls_memory);
    thread->self = thread;
    thread->tls = tls_from_tls_region(tls_memory);
    thread->tls_region = tls_memory;
    thread->errno = 0;
    thread->ipc_buffer = ipc_buffer;
    thread->tcb = tcb;
    thread->cnode = cnode;
    thread->vspace = vspace;
    thread->asid_pool = asid_pool;

    memcpy(thread->tls, env.tls.image, env.tls.image_size);

    return TP_ADJ(thread);
}

void *sel4runtime_move_initial_tls(void *tls_memory) {
    if (
        sel4runtime_initial_tls_enabled() ||
        tls_memory == NULL ||
        sel4runtime_thread_tcb() == seL4_CapNull
    ) {
        return NULL;
    }

    thread_t *thread = thread_from_tls_region(tls_memory);
    *thread = *env.initial_thread;
    thread->self = thread;
    thread->tls = tls_from_tls_region(tls_memory);
    thread->tls_region = tls_memory;

    memcpy(thread->tls, env.tls.image, env.tls.image_size);

    seL4_Error err = seL4_TCB_SetTLSBase(
        sel4runtime_thread_tcb(),
        (uintptr_t)TP_ADJ(thread)
    );
    if (err != seL4_NoError) {
        return NULL;
    }

    env.initial_thread = NULL;
    return TP_ADJ(thread);
}

thread_t *__sel4runtime_thread_self(void) {
    if (!sel4runtime_initial_tls_enabled()) {
        return env.initial_thread;
    } else {
        return __sel4runtime_tls_self();
    }
}

void __sel4runtime_load_env(
    char const *arg0,
    char const * const *envp,
    auxv_t const auxv[]
) {
    parse_auxv(auxv);
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

#if defined(CONFIG_DEBUG_BUILD)
    seL4_CPtr tcb = __sel4runtime_thread_self()->tcb;
    if (tcb != seL4_CapNull) {
        seL4_DebugNameThread(tcb, env.process_name);
    }
#endif
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
            parse_phdr(auxv[i].a_un.a_ptr);
            break;
        }
        case AT_SEL4_BOOT_INFO: {
            seL4_BootInfo *bootinfo = auxv[i].a_un.a_ptr;
            if (bootinfo == NULL) break;
            env.bootinfo = bootinfo;
            thread_t *thread = __sel4runtime_thread_self();
            thread->ipc_buffer = bootinfo->ipcBuffer;
            thread->ipc_buffer_page = seL4_CapInitThreadIPCBuffer;
            thread->tcb = seL4_CapInitThreadTCB;
            thread->cnode = seL4_CapInitThreadCNode;
            thread->vspace = seL4_CapInitThreadVSpace;
            thread->asid_pool = seL4_CapInitThreadASIDPool;
            break;
        }
        default:
            break;
        }
    }
}

static void parse_phdr(Elf_Phdr* header) {
    switch (header->p_type) {
        case PT_TLS:
            load_tls_data(header);
            break;
        default:
            break;
    }
}

static void load_tls_data(Elf_Phdr *header) {
    env.tls.image = (void *) header->p_vaddr;
    env.tls.align = header->p_align;
    env.tls.image_size = header->p_filesz;
    env.tls.memory_size = header->p_memsz;
    env.tls.region_size
        = ROUND_UP(sizeof (thread_t), env.tls.align)
        + (2 * sizeof (void *))
        + GAP_ABOVE_TP
        + ROUND_UP(header->p_memsz, env.tls.align);
}

static void try_init_static_tls(void) {
    if (
        sel4runtime_thread_tcb() != seL4_CapNull &&
        env.tls.region_size <= sizeof(static_tls)
    ) {
        sel4runtime_move_initial_tls(static_tls);
    }
}

static thread_t *thread_from_tls_region(void *tls_region) {
#if defined(TLS_ABOVE_TP)
    uintptr_t thread
        = (uintptr_t)tls_region
        + ROUND_UP(sizeof(thread_t), env.tls.align)
        - sizeof(thread_t);
#else
    uintptr_t thread
        = (uintptr_t)tls_region
        + env.tls.region_size
        - ROUND_UP(sizeof(thread_t), env.tls.align);
#endif
    return (thread_t *)thread;
}

static char *tls_from_tls_region(void *tls_region) {
#if defined(TLS_ABOVE_TP)
    uintptr_t tls
        = (uintptr_t)tls_region
        + ROUND_UP(sizeof(thread_t), env.tls.align)
        + GAP_ABOVE_TP;
#else
    uintptr_t tls
        = (uintptr_t)tls_region
        + env.tls.region_size
        - ROUND_UP(sizeof(thread_t), env.tls.align)
        - env.tls.memory_size;
#endif
    return (char *)tls;
}
