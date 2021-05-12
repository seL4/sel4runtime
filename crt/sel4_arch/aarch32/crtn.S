/*
 * Copyright 2019, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
.syntax unified

.section .init
	ldmia sp!, {fp, lr}
	bx lr

.section .fini
	ldmia sp!, {fp, lr}
	bx lr
