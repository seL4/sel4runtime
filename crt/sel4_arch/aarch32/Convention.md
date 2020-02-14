<!--
     Copyright 2019, Data61
     Commonwealth Scientific and Industrial Research Organisation (CSIRO)
     ABN 41 687 119 230.

     This software may be distributed and modified according to the terms of
     the BSD 2-Clause license. Note that NO WARRANTY is provided.
     See "LICENSE_BSD2.txt" for details.

     @TAG(DATA61_BSD)
-->
# Calling Conventions for AARCH32

* `sp` contains the stack pointer,
* `lr` (link register) contains the return address,
* `a1` to `a4` are used to store the arguments and results of a
  subroutine.

Additionally:

* The stack must always be double-word aligned, i.e. `sp` mod 8 == 0.

## Prologue

1. push the value of `lr` onto the stack,
2. push the value of `fp` onto the stack, then
3. read the current value of `sp` into `fp`.

```asm
stmdb sp!, {fp, lr}
mov fp, sp
```

## Epilogue

```asm
ldmia sp!, {fp, lr}
bx lr
```
