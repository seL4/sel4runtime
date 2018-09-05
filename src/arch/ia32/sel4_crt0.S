#include "autoconf.h"

.section .text
.global _sel4_start
_sel4_start:
	leal __thread_top, %esp
	jmp __sel4_start_root

.section .bss
__thread_base:
	.align 16
	.space CONFIG_SEL4RUNTIME_ROOT_STACK
__thread_top:
