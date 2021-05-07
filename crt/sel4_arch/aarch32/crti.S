/*
 * Copyright 2019, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
.syntax unified

.section .init
.global _init
_init:
	stmdb sp!, {fp, lr}
	mov fp, sp

.section .fini
.global _fini
_fini:
	stmdb sp!, {fp, lr}
	mov fp, sp
