#include <sel4runtime.h>
#include <sel4/sel4.h>

#undef NULL
#include "auxv.h"
#include "start.h"

// The seL4 runtime environment.
static struct {
    char const *process_name;

    // cspace descriptors
    sel4runtime_cspace_t cspace;

    // ELF Headers
    struct {
        size_t count;
        size_t size;
    } program_header;

    // Auxiliary vector
    auxv_t const *auxv;

    // Environment vector
    char const * const *envp;
} env;

static void name_process(char const *name);
static void parse_auxv(auxv_t const auxv[]);

char const *sel4runtime_process_name(void) {
    return env.process_name;
}

const sel4runtime_cspace_t *sel4runtime_cspace_descriptor(void) {
    return &env.cspace;
}

void __sel4runtime_load_env(
    char const *arg0,
    char const * const *envp,
    auxv_t const auxv[]
) {
    name_process(arg0);
    parse_auxv(auxv);

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
        case AT_PHENT:
            env.program_header.size = auxv[i].a_un.a_val;
            break;
        case AT_PHNUM:
            env.program_header.count = auxv[i].a_un.a_val;
            break;
        case AT_SEL4_BOOT_INFO:
            env.cspace.tag = SEL4RUNTIME_CSPACE_BOOTINFO;
            env.cspace.bootinfo.cspace = auxv[i].a_un.a_ptr;
            break;
        default:
            break;
        }
    }
}
