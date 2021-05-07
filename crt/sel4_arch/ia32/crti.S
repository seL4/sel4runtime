/*
 * Copyright 2019, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
.section .init
.global _init
_init:
	pushl %ebp
	movl  %esp, %ebp
	// Align the stack to 16-bytes after return address pushed by call
	// and the ebp pushed above.
	sub  $0x8, %esp

.section .fini
.global _fini
_fini:
	pushl %ebp
	movl  %esp, %ebp
	// Align the stack to 16-bytes after return address pushed by call
	// and the ebp pushed above.
	sub  $0x8, %esp
