.section .init
.global _init
_init:
	pushq %rbp
	movq  %rsp, %rbp

.section .fini
.global _fini
_fini:
	pushq %rbp
	movq  %rsp, %rbp
