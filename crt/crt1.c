typedef struct {
	int a_type;
	union {
		long a_val;
		void *a_ptr;
		void (*a_fnc)(void);
	} a_un;
} auxv_t;

/// The seL4rt entrypoint.
void __sel4rt_start_main(
	unsigned long argc,
	char **argv,
	char **envp,
	auxv_t auxv[]
);

#ifndef NULL
#define NULL ((void *) 0)
#endif

/*
 * This function is simply passed a pointer to the inital stack from the
 * C runtime entrypoint.
 *
 * The stack has the following structure:
 *
 * * argument count,
 * * array of argument pointers,
 * * an empty string,
 * * array of environment pointers,
 * * a null terminator,
 * * array of auxiliary vector entries,
 * * an 'zero' auxiliary vector, then
 * * unspecified data.
 */
void _start_c(void *stack) {
	// First word on the stack is argument count.
	unsigned long argc = *((unsigned long *) stack);

	// Second word on the stack is the start of the argument vector.
	char **argv = &((char **) stack)[1];

	// The environment pointer vector follows after the argv.
	char **envp = &argv[argc + 1];
	int envc = 0;
	while (envp[envc] != NULL) {
		envc++;
	}

	// The auxiliary vector follows the environment pointer vector.
	auxv_t *auxv = (void *)(&envp[envc + 1]);

	__sel4rt_start_main(argc, argv, envp, auxv);
}
