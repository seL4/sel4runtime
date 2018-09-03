.section .init
	addi  sp, s0, 0
	ld    ra, 8(sp)
	ld    s0, 0(sp)
	addi  sp, sp, 16
	ret


.section .fini
	addi  sp, s0, 0
	ld    ra, 8(sp)
	ld    s0, 0(sp)
	addi  sp, sp, 16
	ret
