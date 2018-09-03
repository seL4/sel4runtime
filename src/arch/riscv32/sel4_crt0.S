.section .text
.global _sel4_start
_sel4_start:
	la sp, __thread_base
	jal __sel4_start_root
	/* should not return */
1:
	j 1b

.section .bss
__thread_base:
	.align 16
	.space 16384
__thread_top:
