#include "elf_helper.h"

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
