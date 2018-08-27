.section .text
.global _start
_start:
	movq %rsp, %rbp

	push 0(%rbp)
	call _start_c

	/* should not return */
1:
	jmp  1b
