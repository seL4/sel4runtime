# Calling Conventions for Intel 64

* `%rsp` contains the stack pointer,
* `%rbp` contains the frame pointer,
* `%rax` contains the return value, and
* `%rbx`and `%r12` to `%r15` are callee-save registers.

Additionally:

* The stack must always be 16-byte aligned, i.e. `%rsp` mod 16 == 0.

# Prologue

1. push `%rbp` onto the stack,
2. read the current value of `%rsp` into `%rbp`

```asm
push %rbp
mov  %rsp, %rbp
```

# Epilogue

```asm
leave
ret
```

# Argument Passing

Arguments are passed on the stack in `%rbp+8+(8 * n)`
