/*
 * Copyright 2019, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
.section .text
.global _start
_start:

/*
 * RISC-V uses the gp (global pointer) to provide a link-time
 * optimisation. It is kept in a location near the most-used symbols in
 * the text segments for any particular section of code.
 *
 * This means that a single instruction can be used to jump to a small
 * offset from the gp rather than 2 or more instructions to do a far
 * jump to an absolute address.
 *
 * The initial location for this is only known at the final link stage
 * before assembly of the static image. This code below uses a
 * replacement from the linker to set the value of the gp.
 *
 * Relaxation must be disabled when this value is set, otherwise parts
 * of this code may be optimised away.
 *
 * https://web.archive.org/web/20170828212605/https://www.sifive.com/blog/2017/08/28/all-aboard-part-3-linker-relaxation-in-riscv-toolchain/
 */
.option push
.option norelax
1:auipc gp, %pcrel_hi(__global_pointer$)
  addi  gp, gp, %pcrel_lo(1b)
.option pop

	li s0, 0
	li ra, 0

	addi  a0, sp, 0
	jal  ra, __sel4_start_c

	/* should not return */
1:
	j 1b
