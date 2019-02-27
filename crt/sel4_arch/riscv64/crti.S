/*
 * Copyright 2019, Data61
 * Commonwealth Scientific and Industrial Research Organisation (CSIRO)
 * ABN 41 687 119 230.
 *
 * This software may be distributed and modified according to the terms of
 * the BSD 2-Clause license. Note that NO WARRANTY is provided.
 * See "LICENSE_BSD2.txt" for details.
 *
 * @TAG(DATA61_BSD)
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
