.section .init
	ldp fp, lr, [sp], #16
	ret

.section .fini
	ldp fp, lr, [sp], #16
	ret
