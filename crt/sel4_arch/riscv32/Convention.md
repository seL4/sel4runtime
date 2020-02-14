<!--
     Copyright 2019, Data61
     Commonwealth Scientific and Industrial Research Organisation (CSIRO)
     ABN 41 687 119 230.

     This software may be distributed and modified according to the terms of
     the BSD 2-Clause license. Note that NO WARRANTY is provided.
     See "LICENSE_BSD2.txt" for details.

     @TAG(DATA61_BSD)
-->
# Calling Conventions for RISC-V 32-bit

* `sp` contains the stack pointer,
* `fp` (`s0`) contains the frame pointer,
* `ra` contains the return address,
* `a0` to `a7` are used to pass the first 8 arguments to a subroutine.
* `a0` to `a1` are used to subroutine return values.

## Prologue

1. push the value of `ra` onto the stack,
2. push the value of `fp` onto the stack, then
3. read the current value of `sp` into `fp`.

```asm
addi  sp, sp, -8
sw    s0, 0(sp)
sw    ra, 4(sp)
addi  s0, sp, 0
```

## Epilogue

```asm
addi  sp, s0, 0
lw    ra, 4(sp)
lw    s0, 0(sp)
addi  sp, sp, 8
ret
```
