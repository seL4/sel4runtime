/* SPDX-License-Identifier: MIT */
/*
 * Copyright © 2005-2014 Rich Felker, et al.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#pragma once

#include <sel4runtime/stdint.h>

/*
 * This is a minimal definition of the ELF headers that is sufficient
 * for sel4runtime to pass through program headers.
 */

typedef sel4runtime_uint16_t Elf32_Half;
typedef sel4runtime_uint16_t Elf64_Half;

typedef sel4runtime_uint32_t Elf32_Word;
typedef sel4runtime_int32_t  Elf32_Sword;
typedef sel4runtime_uint32_t Elf64_Word;
typedef sel4runtime_int32_t  Elf64_Sword;

typedef sel4runtime_uint64_t Elf32_Xword;
typedef sel4runtime_int64_t  Elf32_Sxword;
typedef sel4runtime_uint64_t Elf64_Xword;
typedef sel4runtime_int64_t  Elf64_Sxword;

typedef sel4runtime_uint32_t Elf32_Addr;
typedef sel4runtime_uint64_t Elf64_Addr;

typedef sel4runtime_uint32_t Elf32_Off;
typedef sel4runtime_uint64_t Elf64_Off;

typedef sel4runtime_uint16_t Elf32_Section;
typedef sel4runtime_uint16_t Elf64_Section;

typedef struct {
    Elf32_Word  p_type;
    Elf32_Off   p_offset;
    Elf32_Addr  p_vaddr;
    Elf32_Addr  p_paddr;
    Elf32_Word  p_filesz;
    Elf32_Word  p_memsz;
    Elf32_Word  p_flags;
    Elf32_Word  p_align;
} Elf32_Phdr;

typedef struct {
    Elf64_Word   p_type;
    Elf64_Word   p_flags;
    Elf64_Off    p_offset;
    Elf64_Addr   p_vaddr;
    Elf64_Addr   p_paddr;
    Elf64_Xword  p_filesz;
    Elf64_Xword  p_memsz;
    Elf64_Xword  p_align;
} Elf64_Phdr;
