#
# Copyright 2019, Data61, CSIRO (ABN 41 687 119 230)
#
# SPDX-License-Identifier: BSD-2-Clause
#

set(SEL4_RUNTIME_DIR "${CMAKE_CURRENT_LIST_DIR}" CACHE STRING "")
mark_as_advanced(SEL4_RUNTIME_DIR)

function(sel4runtime_import_project)
    add_subdirectory(${SEL4_RUNTIME_DIR} sel4runtime)
endfunction()

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(sel4runtime DEFAULT_MSG SEL4_RUNTIME_DIR)
