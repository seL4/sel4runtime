<!--
     Copyright 2019, Data61
     Commonwealth Scientific and Industrial Research Organisation (CSIRO)
     ABN 41 687 119 230.

     This software may be distributed and modified according to the terms of
     the BSD 2-Clause license. Note that NO WARRANTY is provided.
     See "LICENSE_BSD2.txt" for details.

     @TAG(DATA61_BSD)
-->
# Calling Conventions for AARCH64

* `sp` contains the stack pointer,
* `lr` (link register) contains the return address,
* `fp` contains the frame pointer,
* `r19` to `r28` are callee-saved and must be reset to their original
  state before returning, and
* `r0` to `r7` are used to store the arguments and results of a
  subroutine.

Additionally:

* The stack must always be quad-word aligned, i.e. `sp` mod 16 == 0.

## Prologue

1. push the value of `lr` onto the stack,
2. push the value of `fp` onto the stack, then
3. read the current value of `sp` into `fp`.

```asm
stp fp, lr, [sp, -16]!
mov fp, sp
```

## Epilogue

```asm
ldp fp, lr, [sp], #16
```
