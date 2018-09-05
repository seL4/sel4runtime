#include <elf/elf.h>

#pragma once

// The elf header is a different structure depending on the word size of
// the elf.
typedef Elf64_Phdr Elf_Phdr;
typedef Elf64_Addr Elf_Addr;
typedef Elf64_Word Elf_Word;
