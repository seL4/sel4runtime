/*
 * Copyright 2019, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
.section .init
.global _init
_init:
	pushq %rbp
	movq  %rsp, %rbp

.section .fini
.global _fini
_fini:
	pushq %rbp
	movq  %rsp, %rbp
