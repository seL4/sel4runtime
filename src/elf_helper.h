#include <elf/elf.h>

#pragma once

// The elf header is a different structure depending on the word size of
// the elf.
#if CONFIG_WORD_SIZE == 32
typedef Elf32_Phdr Elf_Phdr;
typedef Elf32_Addr Elf_Addr;
typedef Elf32_Word Elf_Word;
#elif CONFIG_WORD_SIZE == 64
typedef Elf64_Phdr Elf_Phdr;
typedef Elf64_Addr Elf_Addr;
typedef Elf64_Word Elf_Word;
#else
#error "CONFIG_WORD_SIZE not configured"
#endif

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
