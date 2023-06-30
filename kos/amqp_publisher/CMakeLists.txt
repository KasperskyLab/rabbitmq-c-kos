# © 2023 AO Kaspersky Lab. All Rights Reserved
# Licensed under the MIT License

cmake_minimum_required (VERSION 3.12)
project (amqp)

# Initialize CMake library for Kaspersky OS SDK.
include (platform)
initialize_platform ()

# Connect artifact installation functions.
include (platform/install)

# Toolchain for work with NK-parser.
include (platform/nk)

# Build edl files
nk_build_edl_files (publisher_edl_files
                    NK_MODULE "amqp"
                    EDL ${CMAKE_CURRENT_SOURCE_DIR}/../resources/edl/Publisher.edl)

nk_build_edl_files (multi_vfs_amqp_env
                    NK_MODULE "amqp"
                    EDL ${CMAKE_CURRENT_SOURCE_DIR}/../resources/edl/Env.edl)

# Attach a package with the virtual file system implementation.
find_package (vfs REQUIRED)
include_directories (${vfs_INCLUDE})

# Attach a package with the dhcpcd entity implementation.
find_package (rump REQUIRED COMPONENTS DHCPCD_ENTITY)
include_directories (${rump_INCLUDE})

# Attach a package with VFS entities implementation
find_package (precompiled_vfs REQUIRED)
include_directories (${precompiled_vfs_INCLUDE})

# Attach the directory with the generated config header files
include_directories (${CMAKE_CURRENT_BINARY_DIR})

# Attach the directories with entities.
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../resources/include)

add_subdirectory (publisher)

if (RUN_TESTS)
    add_subdirectory (tests)
endif ()

# Attach the directory with the Einit entity to create a solution image
# based on Kaspersky OS.
add_subdirectory (einit)
