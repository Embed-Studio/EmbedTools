cmake_minimum_required(VERSION 3.16)
project(EmbedTools C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

option(EMBEDTOOLS_ARM_CORTEX_M "Build Cortex-M platform modules (time_dwt)" OFF)
# Host tests only by default when EmbedTools is the top-level project.
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    set(_embedtools_tests_default ON)
else()
    set(_embedtools_tests_default OFF)
endif()
option(EMBEDTOOLS_BUILD_TESTS "Build host tests" ${_embedtools_tests_default})

# Portable core
add_library(embedtools_time_cycles STATIC src/time_cycles.c)
target_include_directories(embedtools_time_cycles PUBLIC include)

# ARM Cortex-M platform modules: select explicitly, cross-compile only.
if(EMBEDTOOLS_ARM_CORTEX_M)
    add_library(embedtools_time_dwt STATIC platform/arm_cortex_m/time_dwt.c)
    target_link_libraries(embedtools_time_dwt PUBLIC embedtools_time_cycles)
endif()

if(EMBEDTOOLS_BUILD_TESTS AND NOT CMAKE_CROSSCOMPILING)
    enable_testing()
    add_executable(test_time_cycles tests/test_time_cycles.c)
    target_link_libraries(test_time_cycles PRIVATE embedtools_time_cycles m)
    target_compile_options(test_time_cycles PRIVATE -Wall -Wextra -Werror)
    # assert() must stay active
    target_compile_options(test_time_cycles PRIVATE -UNDEBUG)
    add_test(NAME time_cycles COMMAND test_time_cycles)
endif()
