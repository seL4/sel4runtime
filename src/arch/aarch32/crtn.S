.syntax unified

.section .init
	ldmia sp!, {fp, lr}
	bx lr

.section .fini
	ldmia sp!, {fp, lr}
	bx lr
