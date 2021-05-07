<!--
     Copyright 2019, Data61, CSIRO (ABN 41 687 119 230)

     SPDX-License-Identifier: CC-BY-SA-4.0
-->

# Calling Conventions for RISC-V 64-bit

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
addi  sp, sp, -16
sd    s0, 0(sp)
sd    ra, 8(sp)
addi  s0, sp, 0
```

## Epilogue

```asm
addi  sp, s0, 0
ld    ra, 8(sp)
ld    s0, 0(sp)
addi  sp, sp, 16
ret
```
