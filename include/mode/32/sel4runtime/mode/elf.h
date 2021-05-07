/*
 * Copyright 2019, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include <sel4runtime/elf.h>
#pragma once

// The elf header is a different structure depending on the word size of
// the elf.
typedef Elf32_Phdr Elf_Phdr;
typedef Elf32_Addr Elf_Addr;
typedef Elf32_Word Elf_Word;
