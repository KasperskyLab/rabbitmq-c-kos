# © 2025 AO Kaspersky Lab
# Licensed under the MIT License

project(einit)

# Include the CMake library named image
# containing the solution image build scripts.
include(platform/image)
include(${KL_SDK_ROOT_PATH}/common/build-sd-image.cmake)

# Set the linker and compiler flags.
project_header_default("STANDARD_GNU_17:YES" "STRICT_WARNINGS:YES")

find_program(NTPD_ENTITY
  NAMES "ntpd" REQUIRED
  NO_CMAKE_PATH
  NO_CMAKE_ENVIRONMENT_PATH
  NO_SYSTEM_ENVIRONMENT_PATH
  )

# Define an ENTITIES variable with a list of programs.
set(ENTITIES
  TestEntity
  ${NTPD_ENTITY}
  ${precompiled_vfsVfsNet}
  ${precompiled_vfsVfsSdCardFs}
  ${rump_DHCPCD_ENTITY}
  )

set(YAML_FILE "src/init.yaml.in")
set(SECURITY_PSL_FILE "src/security.psl.in")
set(IMAGE_FS "${CMAKE_BINARY_DIR}/hdd")
set(LIBS_PATH "${IMAGE_FS}/lib")
set(ROOTFS_IMAGE "sdcard0.img")
set(QEMU_FLAGS "-drive file=${ROOTFS_IMAGE},if=sd,format=raw")
set(QEMU_DEPENDENCIES ${ROOTFS_IMAGE})

# Add hosts file to romfs image, to be used by application.
set(FILES ${RESOURCES}/rootfs/etc/hosts)

set(EXTRA_CONNECTIONS
"  - target: kl.VfsNet
    id: kl.VfsNet
  - target: kl.VfsSdCardFs
    id: kl.VfsSdCardFs"
  )

set_target_properties(${rump_DHCPCD_ENTITY} PROPERTIES
  DEPENDS_ON_ENTITY ""
  EXTRA_CONNECTIONS ${EXTRA_CONNECTIONS}
  EXTRA_ENV "\
    VFS_FILESYSTEM_BACKEND: client{fs->net}:kl.VfsSdCardFs
    VFS_NETWORK_BACKEND: client:kl.VfsNet"
  EXTRA_ARGS "\
    - '-4'
    - -f
    - /etc/dhcpcd.conf"
  )

set_target_properties(${precompiled_vfsVfsSdCardFs} PROPERTIES
  EXTRA_ARGS "\
    - -l
    - romfs /romfs romfs ro
    - -l
    - nodev /var ramfs 0
    - -l
    - devfs /dev devfs 0"
  )

set_target_properties(${precompiled_vfsVfsNet} PROPERTIES
  EXTRA_ARGS "\
    - -l
    - devfs /dev devfs 0
    - -l
    - romfs /etc romfs ro"
  )

# Add a target to create filesystem for disk image.
add_custom_target(disk_image_fs
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${RESOURCES}/rootfs ${IMAGE_FS}
  )

# Add a target to create disk image.
add_custom_target(${ROOTFS_IMAGE}
  DEPENDS disk_image_fs
  COMMAND ${KL_SDK_ROOT_PATH}/common/prepare_hdd_img.sh -d ${IMAGE_FS} -s 64 -f fat32 -img ${ROOTFS_IMAGE}
  )

# Create a top-level CMake target named kos-image which can be used
# when building a solution image to be started on the hardware platform.
build_kos_hw_image(kos-image
  EINIT_ENTITY EinitHw
  CONNECTIONS_CFG ${YAML_FILE}
  SECURITY_PSL ${SECURITY_PSL_FILE}
  IMAGE_FILES ${ENTITIES} ${FILES}
  )

add_dependencies(kos-image disk_image_fs)

# Create a top-level CMake target named sd-image which can be used
# when building an SD card image to be started on the hardware platform.
build_sd_image(sd-image
  KOS_IMAGE_TARGET kos-image
  IMAGE_FS ${IMAGE_FS}
  )

# Create a top-level CMake target named kos-qemu-image which
# can be used when building a solution image to be started in QEMU.
build_kos_qemu_image(kos-qemu-image
  EINIT_ENTITY EinitQemu
  CONNECTIONS_CFG ${YAML_FILE}
  SECURITY_PSL  ${SECURITY_PSL_FILE}
  QEMU_FLAGS "${QEMU_FLAGS}"
  QEMU_DEPENDENCIES ${QEMU_DEPENDENCIES}
  IMAGE_FILES ${ENTITIES} ${FILES}
  )
