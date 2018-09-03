.section .text
.global _sel4_start
_sel4_start:
	leaq __thread_top, %rsp
	jmp __sel4_start_root

.section .bss
__thread_base:
	.align 16
	.space 16384
__thread_top:
