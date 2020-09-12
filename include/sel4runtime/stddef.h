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

#include <sel4runtime/stdint.h>

#pragma once

/*
 * This is a very naive implementation of stddef that should work for
 * most major platforms. This is the minimal subset needed for the
 * runtime.
 */

typedef sel4runtime_uintptr_t sel4runtime_size_t;

#define SEL4RUNTIME_NULL ((void *)0)
