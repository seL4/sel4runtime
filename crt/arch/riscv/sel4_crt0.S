/*
 * Copyright 2019, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include <sel4runtime/gen_config.h>

.section .text
.global _sel4_start
_sel4_start:

/* Set gp for relaxation. See
 * https://www.sifive.com/blog/2017/08/28/all-aboard-part-3-linker-relaxation-in-riscv-toolchain/
 */
.option push
.option norelax
1:auipc gp, %pcrel_hi(__global_pointer$)
  addi  gp, gp, %pcrel_lo(1b)
.option pop

	la sp, __stack_top
	jal __sel4_start_root
	/* should not return */
1:
	j 1b

.section .bss
__stack_base:
	.align 16
	.space CONFIG_SEL4RUNTIME_ROOT_STACK
__stack_top:
