/*
 * Copyright 2019, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include <sel4runtime/gen_config.h>

.syntax unified

.section .text
.global _sel4_start
_sel4_start:
	ldr sp, =__stack_top
	bl __sel4_start_root
	/* should not return */
1:
	b 1b

.section .bss
__stack_base:
	.align 16
	.space CONFIG_SEL4RUNTIME_ROOT_STACK
__stack_top:
