.section .text
.global _start
_start:
	movl %esp, %ebp

	push 0(%ebp)
	call __sel4_start_c

	/* should not return */
1:
	jmp  1b
