.syntax unified

.section .text
.global _sel4_start
_sel4_start:
	ldr sp, =__thread_base
	bl __sel4_start_root
	/* should not return */
1:
	b 1b

.section .bss
__thread_base:
	.align 16
	.space 16384
__thread_top:
