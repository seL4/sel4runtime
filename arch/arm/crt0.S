.syntax unified

.section .text
.global _start
_start:
	mov fp, 0
	mov lr, 0

	mov a1, sp
	bl _start_c

	/* should not return */
1:
	b 1b
