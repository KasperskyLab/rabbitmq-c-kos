#!/usr/bin/env bash

# © 2024 AO Kaspersky Lab
# Licensed under the MIT License

set -e

usage() {
    echo "Usage: $0 <TARGET> [SDK_PATH] "
    echo ""
    echo "Available targets":
    echo "  qemu     - build for QEMU."
    echo "  rpi      - build for Raspberry Pi 4 B."
    echo ""
    echo "Optional arguments:"
    echo "  SDK_PATH - Path to the KasperskyOS SDK."
    echo ""
    echo "Examples:"
    echo "  ./cross-build.sh qemu"
    echo "  ./cross-build.sh rpi \"/opt/KasperskyOS-Community-Edition-<version>\""
    echo ""
}

SDK_PATH=""

if [ $# -eq 0 ]
then
    usage
    exit 0
fi

if [ "$1" == "qemu" ]
then
    TARGET_PLATFORM="sim"
elif [ "$1" == "rpi" ]
then
    TARGET_PLATFORM="sd-image"
else
    echo "Unknown target platform is specified."
    usage
    exit 1
fi

if [ $# -eq 2 ]
then
   SDK_PATH="$2"
fi

SCRIPT_DIR="$(dirname "$(realpath "$0")")"
BUILD="${SCRIPT_DIR}/build"

export LANG=C
export TARGET="aarch64-kos"
export PKG_CONFIG=""
export INSTALL_PREFIX="$BUILD/../install"

[ ! -z $SDK_PATH ] && export SDK_PREFIX=$SDK_PATH

if [ -z "$SDK_PREFIX" ]
then
    echo "Path to the KasperskyOS SDK is not specified."
    usage
    exit 1
fi

export PATH="$SDK_PREFIX/toolchain/bin:$PATH"

export BUILD_WITH_CLANG=
export BUILD_WITH_GCC=

TOOLCHAIN_SUFFIX=""

if [ "$BUILD_WITH_CLANG" == "y" ];then
    TOOLCHAIN_SUFFIX="-clang"
fi

if [ "$BUILD_WITH_GCC" == "y" ];then
    TOOLCHAIN_SUFFIX="-gcc"
fi

"$SDK_PREFIX/toolchain/bin/cmake" -G "Unix Makefiles" -B "$BUILD" \
      -D CMAKE_BUILD_TYPE:STRING=Debug \
      -D CMAKE_INSTALL_PREFIX:STRING="$INSTALL_PREFIX" \
      -D BOARD:STRING="RPI4_BCM2711" \
      -D CMAKE_EXTERNAL_PREFIX:STRING="$PWD/../.." \
      -D CMAKE_FIND_ROOT_PATH="$PREFIX_DIR/sysroot-$TARGET" \
      -D CMAKE_TOOLCHAIN_FILE="$SDK_PREFIX/toolchain/share/toolchain-$TARGET$TOOLCHAIN_SUFFIX.cmake" \
      "$SCRIPT_DIR/" && \
"$SDK_PREFIX/toolchain/bin/cmake" --build "$BUILD" --target $TARGET_PLATFORM
