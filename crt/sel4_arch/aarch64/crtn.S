/*
 * Copyright 2019, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
.section .init
	ldp fp, lr, [sp], #16
	ret

.section .fini
	ldp fp, lr, [sp], #16
	ret
