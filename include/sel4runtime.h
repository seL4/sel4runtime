/*
 * The seL4 run-time interface.
 *
 * This provides an interface to the values managed by sel4runtime.
 */

/*
 * Get the name of the process.
 *
 * Returns the basename of the first argument in argv.
 */
char const *sel4runtime_process_name(void);
