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
