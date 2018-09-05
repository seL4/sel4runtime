.section .text
.global _start
_start:
	li s0, 0
	li ra, 0

	addi  a0, sp, 0
	jal  ra, __sel4_start_c

	/* should not return */
1:
	j 1b
