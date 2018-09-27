/*
 * Copyright 2018, Data61
 * Commonwealth Scientific and Industrial Research Organisation (CSIRO)
 * ABN 41 687 119 230.
 *
 * This software may be distributed and modified according to the terms of
 * the BSD 2-Clause license. Note that NO WARRANTY is provided.
 * See "LICENSE_BSD2.txt" for details.
 *
 * @TAG(DATA61_BSD)
 */
.section .text
.global _start
_start:
	mov %esp, %ebp

	push %ebp
	push %ebp
	call __sel4_start_c

	/* should not return */
1:
	jmp  1b
