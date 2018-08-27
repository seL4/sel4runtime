.section .text
.global _start
_start:
	mov fp, #0
	mov lr, #0

	mov x0, sp
	bl _start_c

	/* should not return */
1:
	b 1b
