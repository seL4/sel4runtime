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
