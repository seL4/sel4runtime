.section .text
.global _start
_start:
	movq %rsp, %rbp

	push 0(%rbp)
	call __sel4_start_c

	/* should not return */
1:
	jmp  1b
