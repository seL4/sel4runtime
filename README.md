# The seL4 Run-time

This provides a minimal runtime for running a C or C-compatible process, 
i.e. one with a C-like `main`, in a minimal seL4 environment.

This runtime provides mechanisms for accessing everything a standard
process would expect to need at start and provides additional utilities
for delegating the creation of processes and threads.

# Standard Processes

All processes (except for the root task) will use the entry-points
provided here as normal and require the `_start` entry-point provided in
the architecture-dependant `crt0.S`. This will then bootstrap into the
runtime entry-point `__sel4_start_c` which simply processes the stack to
find the argument, environment, and auxiliary vectors.

The found vectors, along with`main`, are passed into
`__sel4_start_main` which configures the runtime before starting
`main`.

# Root Task

The root task requires an alternate entry-point `_sel4_start` which
assumes that the `seL4_BootInfo` argument has been passed to it and that
it has not been given a stack.

This entry-point moves onto a static 16 kilobyte stack before invoking
`__sel4_start_root`, which constructs the argument, environment, and
auxiliary vectors. It then passes the constructed vectors, along with
`main`, into `__sel4_start_main` which configures the runtime before
starting `main`.
