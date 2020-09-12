/*
 * Copyright 2019, Data61
 * Commonwealth Scientific and Industrial Research Organisation (CSIRO)
 * ABN 41 687 119 230.
 *
 * This software may be distributed and modified according to the terms of
 * the BSD 2-Clause license. Note that NO WARRANTY is provided.
 * See "LICENSE_BSD2.txt" for details.
 *
 * @TAG(DATA61_BSD)
 */
#include <sel4runtime/elf.h>

#pragma once

// The elf header is a different structure depending on the word size of
// the elf.
typedef Elf64_Phdr Elf_Phdr;
typedef Elf64_Addr Elf_Addr;
typedef Elf64_Word Elf_Word;
