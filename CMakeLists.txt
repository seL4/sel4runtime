# Compile CRT for given arch and get crt0.o, crti.o, and crtn.o.
# Add crt0.o and crti.o to the start of all targets link flags.
# Add crtn.o to the end of all targets link flags.

# Compile sel4runtime library from src.
# Use src as private includes for crt and sel4runtime.
# Use include as public includes for crt and sel4runtime.

cmake_minimum_required(VERSION 3.5.1)

project(sel4runtime C)

set(configure_string "")
add_config_library(sel4runtime "${configure_string}")

string(REPLACE " " ";" asm_flags ${CMAKE_ASM_FLAGS})

add_custom_command(
	OUTPUT
		"${CMAKE_BINARY_DIR}/lib/crt0.o"
        "${CMAKE_BINARY_DIR}/lib/crti.o"
        "${CMAKE_BINARY_DIR}/lib/crtn.o"
	COMMAND ${CMAKE_ASM_COMPILER} ${asm_flags} -c
		-o ${CMAKE_BINARY_DIR}/lib/crt0.o
		${CMAKE_CURRENT_SOURCE_DIR}/src/arch/${KernelSel4Arch}/crt0.S
	COMMAND ${CMAKE_ASM_COMPILER} ${asm_flags} -c
		-o ${CMAKE_BINARY_DIR}/lib/crti.o
		${CMAKE_CURRENT_SOURCE_DIR}/src/arch/${KernelSel4Arch}/crti.S
	COMMAND ${CMAKE_ASM_COMPILER} ${asm_flags} -c
		-o ${CMAKE_BINARY_DIR}/lib/crtn.o
		${CMAKE_CURRENT_SOURCE_DIR}/src/arch/${KernelSel4Arch}/crtn.S
)
add_custom_target(
	sel4runtime_crt
	DEPENDS
		"${CMAKE_BINARY_DIR}/lib/crt0.o"
        "${CMAKE_BINARY_DIR}/lib/crti.o"
        "${CMAKE_BINARY_DIR}/lib/crtn.o"
)

# The sel4runtime library.
add_library(
	sel4runtime STATIC
	src/arch/${KernelSel4Arch}/sel4_crt0.S
	src/crt1.c
	src/start.c
	src/start_root.c
)
target_include_directories(
	sel4runtime
	PRIVATE src
	PUBLIC  include
)
add_dependencies(sel4runtime sel4runtime_crt)

# Currently, elf is required to interpret elf program headers for
# regions such as the TLS. The elf definitions require an implementation
# of `stdint.h` which is correct for the given platform and `muslc`
# currently provides this most conveniently.

# TODO: Remove muslc dependency
target_link_libraries(sel4runtime Configuration sel4 elf muslc)

