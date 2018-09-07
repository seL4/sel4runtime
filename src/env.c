#include <sel4runtime.h>

#include "start.h"
#include "elf_helper.h"

// The seL4 runtime environment.
static struct {
    char const *process_name;
} env;

static void name_process(char *name);

char const *sel4runtime_process_name(void) {
    return env.process_name;
}

void __sel4runtime_load_env(
    char *arg0,
    char **envp,
    auxv_t auxv[]
) {
    name_process(arg0);
}

static void name_process(char *name) {
    env.process_name = name;

    while (*name != '\0') {
        if (*name == '/') {
            env.process_name = name + 1;
        }

        name++;
    }
}
