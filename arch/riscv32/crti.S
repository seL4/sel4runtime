.section .init
.global _init
_init:
	addi  sp, sp, -8
	sw    s0, 0(sp)
	sw    ra, 4(sp)
	addi  s0, sp, 0

.section .fini
.global _fini
_fini:
	addi  sp, sp, -8
	sw    s0, 0(sp)
	sw    ra, 4(sp)
	addi  s0, sp, 0
