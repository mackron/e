cmake_minimum_required(VERSION 3.10)

# Version
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/e.h" E_VERSION_MAJOR_LINE REGEX "#define E_VERSION_MAJOR")
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/e.h" E_VERSION_MINOR_LINE REGEX "#define E_VERSION_MINOR")
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/e.h" E_VERSION_PATCH_LINE REGEX "#define E_VERSION_PATCH")

string(REGEX MATCH "[0-9]+" E_VERSION_MAJOR "${E_VERSION_MAJOR_LINE}")
string(REGEX MATCH "[0-9]+" E_VERSION_MINOR "${E_VERSION_MINOR_LINE}")
string(REGEX MATCH "[0-9]+" E_VERSION_PATCH "${E_VERSION_PATCH_LINE}")

set(E_VERSION "${E_VERSION_MAJOR}.${E_VERSION_MINOR}.${E_VERSION_PATCH}")
message(STATUS "e version: ${E_VERSION}")

project(e VERSION ${E_VERSION})


# Options
option(E_BUILD_EXAMPLES "Build e examples"            OFF)
option(E_BUILD_TESTS    "Build e tests"               OFF)
option(E_FORCE_CXX      "Force compilation as C++"    OFF)
option(E_FORCE_C89      "Force compilation as C89"    OFF)

# Construct compiler options.
set(COMPILE_OPTIONS)

if(E_FORCE_CXX AND E_FORCE_C89)
    message(FATAL_ERROR "E_FORCE_CXX and E_FORCE_C89 cannot be enabled at the same time.")
endif()

if(E_FORCE_CXX)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        message(STATUS "Compiling as C++ (GNU/Clang)")
        list(APPEND COMPILE_OPTIONS -x c++)
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        message(STATUS "Compiling as C++ (MSVC)")
        list(APPEND COMPILE_OPTIONS /TP)
    else()
        message(WARNING "E_FORCE_CXX is enabled but the compiler does not support it. Ignoring.")
    endif()
endif()

if(E_FORCE_C89)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        message(STATUS "Compiling as C89")
        list(APPEND COMPILE_OPTIONS -std=c89)
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        message(WARNING "MSVC does not support forcing C89. E_FORCE_C89 ignored.")
    else()
        message(WARNING "E_FORCE_C89 is enabled but the compiler does not support it. Ingoring.")
    endif()
endif()

# Warnings
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    list(APPEND COMPILE_OPTIONS -Wall -Wextra -Wpedantic)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    #list(APPEND COMPILE_OPTIONS /W4)
endif()


# Link libraries
set(COMMON_LINK_LIBRARIES)

if (UNIX)
    list(APPEND COMMON_LINK_LIBRARIES dl)      # For dlopen(), etc. Most compilers will link to this by default, but some may not.
    list(APPEND COMMON_LINK_LIBRARIES pthread) # Some compilers will not link to pthread by default so list it here just in case.
    list(APPEND COMMON_LINK_LIBRARIES m)

    # If we're compiling for 32-bit ARM we need to link to -latomic.
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "^arm" AND NOT CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64")
        list(APPEND COMMON_LINK_LIBRARIES atomic)
    endif()
endif()


# Common interface
add_library(e_common INTERFACE)
target_compile_options(e_common INTERFACE ${COMPILE_OPTIONS})
target_link_libraries (e_common INTERFACE ${COMMON_LINK_LIBRARIES})


# Main library
add_library(e STATIC
    e.c
    e.h
)

target_include_directories(e PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries     (e PRIVATE e_common)
