/*
 * Copyright 2019, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
.section .init
	addi  sp, s0, 0
	lw    ra, 4(sp)
	lw    s0, 0(sp)
	addi  sp, sp, 8
	ret


.section .fini
	addi  sp, s0, 0
	lw    ra, 4(sp)
	lw    s0, 0(sp)
	addi  sp, sp, 8
	ret
