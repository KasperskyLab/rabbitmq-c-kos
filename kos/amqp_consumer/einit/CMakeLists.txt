# © 2023 AO Kaspersky Lab. All Rights Reserved
# Licensed under the MIT License

project (einit)

# Toolkit for working with NK parser.
include (platform/image)

# Set compilation flags.
project_header_default ("STANDARD_GNU_11:YES" "STRICT_WARNINGS:NO")

find_program (NTPD_ENTITY
              NAMES "ntpd" REQUIRED
              NO_CMAKE_PATH
              NO_CMAKE_ENVIRONMENT_PATH
              NO_SYSTEM_ENVIRONMENT_PATH)

# Define an ENTITIES variable with a list of entities.
set (ENTITIES
    Consumer
    ${precompiled_vfsVfsNet}
    ${NTPD_ENTITY}
    ${rump_DHCPCD_ENTITY})

set (ROOTFS_IMAGE "sdcard0.img")
set (ROOTFS_ENTITY "kl.VfsSdCardFs")
set (FS_BACKEND ${precompiled_vfsVfsSdCardFs})
set (QEMU_FLAGS "-drive file=${ROOTFS_IMAGE},if=sd,format=raw")
set (FILES ${CMAKE_BINARY_DIR}/einit/${ROOTFS_IMAGE})

list (APPEND ENTITIES ${FS_BACKEND})

set (EXTRA_CONNECTIONS
"  - target: kl.VfsNet
    id: kl.VfsNet
  - target: ${ROOTFS_ENTITY}
    id: ${ROOTFS_ENTITY}")

set_target_properties (${rump_DHCPCD_ENTITY} PROPERTIES
    DEPENDS_ON_ENTITY ""
    EXTRA_CONNECTIONS ${EXTRA_CONNECTIONS}
    EXTRA_ENV "\
    VFS_FILESYSTEM_BACKEND: client{fs->net}:${ROOTFS_ENTITY}
    VFS_NETWORK_BACKEND: client:kl.VfsNet"
    EXTRA_ARGS "\
    - '-4'
    - -f
    - /etc/dhcpcd.conf")

set_target_properties (${FS_BACKEND} PROPERTIES
    EXTRA_ARGS "\
    - -l
    - romfs /romfs romfs 0
    - -l
    - nodev /var ramfs 0
    - -l
    - devfs /dev devfs 0")

set_target_properties (${precompiled_vfsVfsNet} PROPERTIES
    EXTRA_ARGS "\
    - -l
    - devfs /dev devfs 0
    - -l
    - romfs /etc romfs 0")

# add hosts file to romfs image, to be used by application
list (APPEND FILES ${CMAKE_SOURCE_DIR}/../resources/rootfs/etc/hosts)

# Add a target with making and copying the disk image to the example build
# directory.
add_custom_command (OUTPUT ${ROOTFS_IMAGE}
                    COMMAND ${CMAKE_COMMAND} -E make_directory
                        ${CMAKE_BINARY_DIR}/hdd
                    COMMAND ${CMAKE_COMMAND} -E copy_directory
                        ${CMAKE_SOURCE_DIR}/../resources/rootfs
                        ${CMAKE_BINARY_DIR}/hdd
                    COMMAND $ENV{SDK_PREFIX}/common/prepare_hdd_img.sh -d
                        ${CMAKE_BINARY_DIR}/hdd -img ${ROOTFS_IMAGE} -f fat32)
add_custom_target (${ROOTFS_IMAGE} DEPENDS ${ROOTFS_IMAGE})

set (SECURITY_PSL_FILE "src/security.psl")

# Building Kaspersky OS solution image for a hardware platform.
build_kos_hw_image (kos-image
                    EINIT_ENTITY EinitHw
                    CONNECTIONS_CFG "src/init.yaml.in"
                    SECURITY_PSL ${SECURITY_PSL_FILE}
                    IMAGE_FILES  ${ENTITIES}
                                 ${FILES})
add_dependencies (kos-image ${ROOTFS_IMAGE})

# Building Kaspersky OS solution image for a QEMU platform.
build_kos_qemu_image (kos-qemu-image
                      EINIT_ENTITY EinitQemu
                      CONNECTIONS_CFG "src/init.yaml.in"
                      SECURITY_PSL ${SECURITY_PSL_FILE}
                      QEMU_FLAGS   ${QEMU_FLAGS}
                      IMAGE_FILES  ${ENTITIES}
                                   ${FILES})
add_dependencies (kos-qemu-image ${ROOTFS_IMAGE})
