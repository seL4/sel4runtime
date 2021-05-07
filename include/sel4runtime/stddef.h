/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: BSD-2-Clause
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
