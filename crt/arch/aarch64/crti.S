.section .init
.global _init
_init:
	stp fp, lr, [sp, -16]!
	mov fp, sp

.section .fini
.global _fini
_fini:
	stp fp, lr, [sp, -16]!
	mov fp, sp
