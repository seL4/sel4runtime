#include <sel4/sel4.h>
#include "start.h"

void __sel4runtime_start_main(
	int (*main)(),
	unsigned long argc,
	char **argv,
	char **envp,
	auxv_t auxv[]
) {
	// Simple start to begin with.
	main(argc, argv, envp);
	while (1) {
		seL4_Yield();
	}
}
