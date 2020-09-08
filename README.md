<!--
     Copyright 2019, Data61
     Commonwealth Scientific and Industrial Research Organisation (CSIRO)
     ABN 41 687 119 230.

     This software may be distributed and modified according to the terms of
     the BSD 2-Clause license. Note that NO WARRANTY is provided.
     See "LICENSE_BSD2.txt" for details.

     @TAG(DATA61_BSD)
-->
# The seL4 Run-time

This repository provides a minimal runtime for running a C or C-compatible process, 
i.e., one with a C-like `main()`, in a minimal seL4 environment.

This runtime provides mechanisms for accessing everything a standard
process or thread would expect to need at start, including thread
local storage.

To use the library, you just need to write a C program that starts
from `main()`, and compile it into an ELF file; the documentation
below gives implementation details.  There is a CMakeLists.txt file
provided to hook sel4runtime into your project.

## Implementation

### Root Task

The root task needs some extra work before `main()` is called.  The
runtime calls `_sel4_start()`, provided by this library, that takes
`seL4_BootInfo` as argument in a register, and has not been given a
stack.

This entry-point sets up a static 16 kilobyte stack before invoking
`__sel4_start_root()`, also provided by the runtime, which
constructs the argument, environment, and auxiliary vectors. It then
passes the constructed vectors, along with the address of `main()`,
into `__sel4_start_main` which configures the runtime according to the
architecture's conventions before starting calling `main()`.

### Standard Processes

All processes other than the root task use the `_start` entry-point provided in
the architecture-dependant `crt0.S`. This will then bootstrap into the
runtime entry-point `__sel4_start_c` which simply processes the stack to
find the argument, environment, and auxiliary vectors.

These vectors, along with the address of `main()`, are passed into
`__sel4_start_main` which configures the runtime before starting
`main`.  The architecture's standard post-main() processing is handled
by `__start`.


### Thread-local storage layout

There are two supported layouts for thread local storage.

Intel X86\_64 and IA32 use a TLS pointer that contains the address
immediately after the TLS region: the region is _below_ it in memory.
Other architectures use a TLS pointer that contains the lowest address in
the TLS.

For X86, memory immediately _above_ the address is available for the C
library to use as thread metadata.  For other architectures, two words are
allocated right at the top of the TLS region.  The first word is
initialised to point to the thread local storage, either at one past
its highest address (X86 family) or at its lowest address (other
architectures). 

In any case, the size of the TLS is determined by values in the ELF
file from which the program is loaded.
