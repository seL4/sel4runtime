.syntax unified

.section .init
.global _init
_init:
	stmdb sp!, {fp, lr}
	mov fp, sp

.section .fini
.global _fini
_fini:
	stmdb sp!, {fp, lr}
	mov fp, sp
