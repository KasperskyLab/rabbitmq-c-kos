# © 2025 AO Kaspersky Lab
# Licensed under the MIT License

cmake_minimum_required(VERSION 3.25)
project(tests)

# Initialize CMake library for the KasperskyOS SDK.
include(platform)
# Statically link executable files.
initialize_platform(FORCE_STATIC)

# Include the CMake library named install,
# which contains artifact installation functions.
include(platform/install)

# Include the CMake library named nk
# for working with the NK compiler (nk-gen-c).
include(platform/nk)

# Detect board.
if(CMAKE_SYSTEM_SOC STREQUAL "BCM2711")
  set(BOARD "RPI4_BCM2711")
elseif(CMAKE_SYSTEM_SOC STREQUAL "RK3568")
  set(BOARD "RADXA_ROCK_3A")
else()
  message(FATAL_ERROR "Unsupported board for SoC '${CMAKE_SYSTEM_SOC}'")
endif()

# Attach the vfs package with the VFS component libraries.
find_package(vfs REQUIRED)
include_directories(${vfs_INCLUDE})

# Attach a package with the Dhcpcd program implementation.
find_package(rump REQUIRED COMPONENTS DHCPCD_ENTITY)
include_directories(${rump_INCLUDE})

# Attach the vfs package with the VFS component executable files.
find_package(precompiled_vfs REQUIRED)
include_directories(${precompiled_vfs_INCLUDE})

# Attach the directory with the generated config header files.
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(RESOURCES ${CMAKE_CURRENT_SOURCE_DIR}/../resources)
set(UTILS ${CMAKE_CURRENT_SOURCE_DIR}/../utils)

# Attach the directories with programs.
include_directories(${RESOURCES}/include)

add_subdirectory(tests)

# Attach the einit directory containing the KasperskyOS image
# to the project build.
add_subdirectory(einit)
