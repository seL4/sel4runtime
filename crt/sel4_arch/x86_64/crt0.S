/*
 * Copyright 2019, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
.section .text
.global _start
_start:
	movq %rsp, %rbp
	movq %rsp, %rdi

	/*
	 * GCC expects that a C function is always entered via a call
	 * instruction and that the stack is 16-byte aligned before such an
	 * instruction (leaving it 16-byte aligned + 1 word from the
	 * implicit push when the function is entered).
	 *
	 * If additional items are pushed onto the stack, the stack must be
	 * manually re-aligned before the call instruction to
	 * __sel4_start_c.
	 */
	subq $0x8, %rsp
	push %rbp
	call __sel4_start_c

	/* should not return */
1:
	jmp  1b
