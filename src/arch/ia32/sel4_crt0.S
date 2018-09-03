.section .text
.global _sel4_start
_sel4_start:
	leal __thread_top, %esp
	jmp __sel4_start_root

.section .bss
__thread_base:
	.align 16
	.space 16384
__thread_top:
