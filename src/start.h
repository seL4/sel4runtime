#pragma once

// Entry into C program.
int main();

#ifndef NULL
#define NULL ((void *) 0)
#endif

typedef struct {
	int a_type;
	union {
		long a_val;
		void *a_ptr;
		void (*a_fnc)(void);
	} a_un;
} auxv_t;

void __sel4rt_start_main(
	int (*main)(),
	unsigned long argc,
	char **argv,
	char **envp,
	auxv_t auxv[]
);
