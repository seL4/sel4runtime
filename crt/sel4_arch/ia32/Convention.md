<!--
     Copyright 2018, Data61
     Commonwealth Scientific and Industrial Research Organisation (CSIRO)
     ABN 41 687 119 230.

     This software may be distributed and modified according to the terms of
     the BSD 2-Clause license. Note that NO WARRANTY is provided.
     See "LICENSE_BSD2.txt" for details.

     @TAG(DATA61_BSD)
-->
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

Arguments are passed on the stack in `%ebp+4+(4 * n)` (between the
return address and the return frame pointer).
