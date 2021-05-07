/*
 * Copyright 2019, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
.section .init
.global _init
_init:
	addi  sp, sp, -16
	sd    s0, 0(sp)
	sd    ra, 8(sp)
	addi  s0, sp, 0

.section .fini
.global _fini
_fini:
	addi  sp, sp, -16
	sd    s0, 0(sp)
	sd    ra, 8(sp)
	addi  s0, sp, 0
