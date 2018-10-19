#include <autoconf.h>
#include <sel4/sel4.h>

#if defined(CONFIG_IPC_BUF_GLOBALS_FRAME)
__attribute__((__visibility__("hidden")))
void *__aeabi_read_tp_c(void) {
    return ((void **)(seL4_GlobalsFrame))[1];
}
#endif
