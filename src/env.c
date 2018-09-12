#include <sel4runtime.h>
#include <sel4/sel4.h>

#include "auxv.h"
#include "start.h"
#include "thread.h"


static thread_t __initial_thread;

// The seL4 runtime environment.
static struct {
    char const *process_name;

    // cspace descriptors
    sel4runtime_cspace_t cspace;

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

char const *sel4runtime_process_name(void) {
    return env.process_name;
}

const sel4runtime_cspace_t *sel4runtime_cspace_descriptor(void) {
    return &env.cspace;
}

thread_t *__sel4runtime_thread_self(void) {
    if (env.initial_thread != NULL) {
        return env.initial_thread;
    } else {
        // TODO: Return pointer to thread object in TLS.
        return NULL;
    }
}

void __sel4runtime_load_env(
    char const *arg0,
    char const * const *envp,
    auxv_t const auxv[]
) {
    parse_auxv(auxv);
    name_process(arg0);

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
        case AT_SEL4_BOOT_INFO: {
            env.cspace.tag = SEL4RUNTIME_CSPACE_BOOTINFO;
            seL4_BootInfo *bootinfo = auxv[i].a_un.a_ptr;
            env.cspace.cspace.bootinfo = bootinfo;
            env.initial_thread->ipc_buffer = bootinfo->ipcBuffer;
            env.initial_thread->tcb = seL4_CapInitThreadTCB;
            env.initial_thread->cnode = seL4_CapInitThreadCNode;
            env.initial_thread->vspace = seL4_CapInitThreadVSpace;
            env.initial_thread->asid_pool = seL4_CapInitThreadASIDPool;
            break;
        }
        default:
            break;
        }
    }
}
