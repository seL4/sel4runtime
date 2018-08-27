# Calling Conventions for IA-32

* `%esp` contains the stack pointer,
* `%ebp` contains the frame pointer,
* `%eax` contains the return value, and
* `%ebx`, `%esi`, and `%edi` are callee-save registers.

Additionally:

* The stack must always be 16-byte aligned, i.e. `%esp` mod 16 == 0.

# Prologue

1. push `%ebp` onto the stack,
2. read the current value of `%esp` into `%ebp`

```asm
push %ebp
mov  %esp, %ebp
```

# Epilogue

```asm
leave
ret
```

# Argument Passing

Arguments are passed on the stack in `%ebp+4+(4 * n)`
