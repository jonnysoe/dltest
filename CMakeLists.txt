cmake_minimum_required(VERSION 3.15.0)
get_filename_component(ProjectId ${CMAKE_CURRENT_SOURCE_DIR} NAME)
string(REPLACE " " "_" ProjectId ${ProjectId})

message(STATUS "CMAKE_GENERATOR: ${CMAKE_GENERATOR}")

# Set local and global CMake policies before declaring project
cmake_policy(SET CMP0054 NEW)           # Treat string as is, do not treat as variables to be expanded
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)   # cached variables take precedence before option command.
set(CMAKE_POLICY_DEFAULT_CMP0079 NEW)   # target not created in current directory hack.

project(${ProjectId} VERSION 0.1.0)

# Enable CCache
# https://stackoverflow.com/a/64600661/19336104
find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    message(STATUS "Using ccache: ${CCACHE_PROGRAM}")
    set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE_PROGRAM})
    set(CMAKE_C_COMPILER_LAUNCHER ${CCACHE_PROGRAM})
endif()

#Set the default build type if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING
        "Choose the type of build: None, Debug, Release, RelWithDebInfo, MinSizeRel"
        FORCE)
else()
    # Case-insensitive correction hack
    string(TOLOWER ${CMAKE_BUILD_TYPE} CMAKE_BUILD_TYPE_LOWER)
    if(CMAKE_BUILD_TYPE_LOWER STREQUAL "release")
        set(CMAKE_BUILD_TYPE "Release")
    elseif(CMAKE_BUILD_TYPE_LOWER STREQUAL "debug")
        set(CMAKE_BUILD_TYPE "Debug")
    elseif(CMAKE_BUILD_TYPE_LOWER STREQUAL "relwithdebinfo")
        set(CMAKE_BUILD_TYPE "RelWithDebInfo")
    elseif(CMAKE_BUILD_TYPE_LOWER STREQUAL "minsizerel")
        set(CMAKE_BUILD_TYPE "MinSizeRel")
    endif()
endif()

if(WIN32)
    add_compile_definitions(_WIN32)
endif()

if(${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
    add_compile_options(
        -Werror=return-type     # Clang by default does not file error when return type does not match function return
        -Wno-int-conversion
        -Wno-visibility
        -Wno-pragma-pack
        -Wno-microsoft-enum-forward-reference
        -Wno-sizeof-pointer-memaccess
        -Wno-implicit-function-declaration
    )

    # Windows LLVM/Clang does not use libstdc++ like the Clang/GCC counterpart,
    # Linux Clang counterpart does not require GCC as base
    if("${CMAKE_CXX_SIMULATE_ID}" MATCHES "MSVC")
        # Need to use quotes around CMAKE_CXX_SIMULATE_ID as not every configuration have this definition
        # https://blog.conan.io/2022/10/13/Different-flavors-Clang-compiler-Windows.html
        message(STATUS "LLVM/Clang outputs natvis debug information by default.")
    elseif(${CMAKE_BUILD_TYPE} MATCHES "Debug")
        # Clang workaround to emit string to libstdc++
        # https://stackoverflow.com/a/44727479/19336104
        message(STATUS "Clang does not output to libstdc++, enabling 'no-limit-debug-info'")
        add_compile_options(-fno-limit-debug-info)
    endif()
endif()

if(${CMAKE_BUILD_TYPE} MATCHES "Debug")
    # Debug mode have weird behaviors relocating symbols in static library if linked by shared library
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)

    if(NOT MSVC)
        # This is for compatibility with MSVC
        # CMake Tools uses "--config Debug" which doesn't have _DEBUG definition
        # this is added instead of explicitly adding -D_DEBUG when calling cmake
        # https://learn.microsoft.com/en-us/cpp/build/reference/md-mt-ld-use-run-time-library?view=msvc-170
        add_compile_definitions(_DEBUG)
    endif()
elseif(${CMAKE_BUILD_TYPE} MATCHES "Release")
    if(NOT MSVC)
        # This is for compatibility with MSVC
        # CMake Tools uses "--config Debug" which doesn't have _DEBUG definition
        # this is added instead of explicitly adding -D_DEBUG when calling cmake
        # https://learn.microsoft.com/en-us/cpp/build/reference/md-mt-ld-use-run-time-library?view=msvc-170
        add_compile_definitions(NDEBUG)
    endif()
endif()

if(MSVC)
    # add_compile_definitions(_CRT_SECURE_NO_WARNINGS)

    # Remove default /W3 and /W4 by CMake MSVC
    # https://stackoverflow.com/a/58711983/19336104
    string(REGEX REPLACE "/W[3|4]" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    string(REGEX REPLACE "  " " " CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    string(REGEX REPLACE "/W[3|4]" "" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
    string(REGEX REPLACE "  " " " CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
endif()

# https://cmake.org/cmake/help/latest/manual/cmake-buildsystem.7.html#output-artifacts
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin) # .exe/.dll for executable
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin) # shared library, .dll (Windows) files are not affected
# Archives are not required in the final output
# set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin) # .lib for linker
set(CMAKE_PDB_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}) # .pdb for debugging

# Use C++17
set(CMAKE_CXX_STANDARD 17)

# Set Common Settings for all libraries
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# ENABLE_EXPORTS ON below doesn't work on Windows
# Export all symbols on main executable to generate an import library for plugin to link in Windows.
# https://stackoverflow.com/a/75525641/19336104
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

# Disable MSVC's ILK incremental build
# https://stackoverflow.com/a/21569552
add_link_options(/INCREMENTAL:NO)

# Common library
add_library(common_obj OBJECT common.cpp)

if(WIN32)
    if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/dlfcn-win32)
        find_package(Git QUIET)
        if(GIT_FOUND)
            message(STATUS "Fetching dlfcn-win32 library")
            execute_process(COMMAND ${GIT_EXECUTABLE} clone https://github.com/dlfcn-win32/dlfcn-win32.git --single-branch --branch master
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                ERROR_QUIET
            )
        endif()
    endif()

    # dlfcn as a static library
    set(BUILD_SHARED_LIBS OFF)
    add_subdirectory(dlfcn-win32)
    set(CMAKE_DL_LIBS dl)
endif()

# Main executable
add_executable(${ProjectId} main.cpp)
target_compile_definitions(${ProjectId} PRIVATE BUILD_COMMON)
set_target_properties(${ProjectId} PROPERTIES ENABLE_EXPORTS ON)    # export all symbols for plugin(s) to call in non-Windows platforms
target_link_libraries(${ProjectId} PUBLIC common_obj ${CMAKE_DL_LIBS})

# Add libplugin
add_subdirectory(libplugin)
add_dependencies(libplugin ${ProjectId})