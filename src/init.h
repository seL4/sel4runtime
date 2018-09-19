/*
 * Constructor and destructor handling.
 *
 * Constructors and destructors allow for running code before `main` and
 * during `exit` respectively.
 */

/*
 * Executes the functions described in the preinit_array, _init, and
 * init_array.
 */
void __sel4runtime_run_constructors(void);

/*
 * Executes the functions described in the fini_array and _fini.
 */
void __sel4runtime_run_destructors(void);
