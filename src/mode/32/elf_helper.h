#include <elf/elf.h>
#pragma once

// The elf header is a different structure depending on the word size of
// the elf.
typedef Elf32_Phdr Elf_Phdr;
typedef Elf32_Addr Elf_Addr;
typedef Elf32_Word Elf_Word;
