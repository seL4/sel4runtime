/*
 * Copyright 2020, Data61
 * Commonwealth Scientific and Industrial Research Organisation (CSIRO)
 * ABN 41 687 119 230.
 *
 * This software may be distributed and modified according to the terms of
 * the BSD 2-Clause license. Note that NO WARRANTY is provided.
 * See "LICENSE_BSD2.txt" for details.
 *
 * @TAG(DATA61_BSD)
 */

#pragma once

/*
 * This is a very naive implementation of stdint that should work for
 * most major platforms. This is the minimal subset needed for the
 * runtime.
 */

_Static_assert(sizeof(char)      == 1, "char is 8 bits");
_Static_assert(sizeof(short)     == 2, "short is 16 bits");
_Static_assert(sizeof(int)       == 4, "int is 32 bits");
_Static_assert(sizeof(long long) == 8, "long long is 64 bits");
_Static_assert(sizeof(long)      == sizeof(void *), "long is word size");

/* Fixed size integer types */
typedef signed char      sel4runtime_int8_t;
typedef signed short     sel4runtime_int16_t;
typedef signed int       sel4runtime_int32_t;
typedef signed long long sel4runtime_int64_t;

typedef unsigned char      sel4runtime_uint8_t;
typedef unsigned short     sel4runtime_uint16_t;
typedef unsigned int       sel4runtime_uint32_t;
typedef unsigned long long sel4runtime_uint64_t;

/* Word-sized integer types */
typedef signed long   sel4runtime_intptr_t;
typedef unsigned long sel4runtime_uintptr_t;
