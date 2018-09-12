#
# Copyright 2018, Data61
# Commonwealth Scientific and Industrial Research Organisation (CSIRO)
# ABN 41 687 119 230.
#
# This software may be distributed and modified according to the terms of
# the BSD 2-Clause license. Note that NO WARRANTY is provided.
# See "LICENSE_BSD2.txt" for details.
#
# @TAG(DATA61_BSD)
#
# Compile CRT for given arch and get crt0.o, crti.o, and crtn.o.
# Add crt0.o and crti.o to the start of all targets link flags.
# Add crtn.o to the end of all targets link flags.

# Compile sel4runtime library from src.
# Use src as private includes for crt and sel4runtime.
# Use include as public includes for crt and sel4runtime.

cmake_minimum_required(VERSION 3.5.1)

project(sel4runtime C)

set(configure_string "")
config_string(Sel4RuntimeRootStack SEL4RUNTIME_ROOT_STACK
	"Size of the initial stack for the root task"
    DEFAULT 16384
    UNQUOTE
)
config_string(Sel4RuntimeStaticTLS SEL4RUNTIME_STATIC_TLS
	"Size of static TLS area for new processes"
    DEFAULT 16384
    UNQUOTE
)
add_config_library(sel4runtime "${configure_string}")

file(
	GLOB crt_files
		crt/arch/${KernelArch}/*
		crt/sel4_arch/${KernelSel4Arch}/*
)

add_library(sel4runtime_crt_obj OBJECT ${crt_files})
target_include_directories(
	sel4runtime_crt_obj
	PRIVATE
		src
		src/mode/${KernelWordSize}
		src/arch/${KernelArch}
		src/sel4_arch/${KernelSel4Arch}
	PUBLIC  include
)
target_link_libraries(sel4runtime_crt_obj Configuration)
add_custom_command(
	OUTPUT
		"${CMAKE_BINARY_DIR}/lib/crt0.o"
        "${CMAKE_BINARY_DIR}/lib/crti.o"
        "${CMAKE_BINARY_DIR}/lib/crtn.o"
	DEPENDS
		sel4runtime_crt_obj
	COMMAND ${CMAKE_COMMAND} -E env cp
		"$<TARGET_OBJECTS:sel4runtime_crt_obj>"
		${CMAKE_BINARY_DIR}/lib/
	COMMAND ${CMAKE_COMMAND} -E env mv
		${CMAKE_BINARY_DIR}/lib/crt0.S.obj
		${CMAKE_BINARY_DIR}/lib/crt0.o
	COMMAND ${CMAKE_COMMAND} -E env mv
		${CMAKE_BINARY_DIR}/lib/crti.S.obj
		${CMAKE_BINARY_DIR}/lib/crti.o
	COMMAND ${CMAKE_COMMAND} -E env mv
		${CMAKE_BINARY_DIR}/lib/crtn.S.obj
		${CMAKE_BINARY_DIR}/lib/crtn.o
	COMMAND_EXPAND_LISTS
)
add_custom_target(
	sel4runtime_crt
	DEPENDS
		"${CMAKE_BINARY_DIR}/lib/crt0.o"
        "${CMAKE_BINARY_DIR}/lib/crti.o"
        "${CMAKE_BINARY_DIR}/lib/crtn.o"
)

file(
	GLOB arch_source
		src/mode/${KernelWordSize}/*
		src/arch/${KernelArch}/*
		src/sel4_arch/${KernelSel4Arch}/*
)

# The sel4runtime library.
add_library(
	sel4runtime STATIC
	${crt_files}
	${arch_source}
	src/crt1.c
	src/start.c
	src/start_root.c
	src/env.c
)
target_include_directories(
	sel4runtime
	PRIVATE
		src
		src/mode/${KernelWordSize}
		src/arch/${KernelArch}
		src/sel4_arch/${KernelSel4Arch}
	PUBLIC  include
)
add_dependencies(sel4runtime Configuration sel4runtime_crt)

# Currently, elf is required to interpret elf program headers for
# regions such as the TLS. The elf definitions require an implementation
# of `stdint.h` which is correct for the given platform and `muslc`
# currently provides this most conveniently.

# TODO: Remove muslc dependency
target_link_libraries(sel4runtime Configuration sel4 elf)

