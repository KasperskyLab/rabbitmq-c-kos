# Unit testing using the GoogleTest framework

These tests were copied from https://github.com/alanxz/rabbitmq-c/tree/master/tests and converted to
gtests.

## Table of contents
- [Unit testing using the GoogleTest framework](#unit-testing-using-the-googletest-framework)
  - [Table of contents](#table-of-contents)
  - [Solution overview](#solution-overview)
    - [List of programs](#list-of-programs)
    - [Initialization description](#initialization-description)
    - [Security policy description](#security-policy-description)
  - [Getting started](#getting-started)
    - [Building and running the tests](#building-and-running-the-tests)
      - [QEMU](#qemu)
      - [Hardware](#hardware)
      - [CMake input files](#cmake-input-files)

## Solution overview

### List of programs

* `TestEntity`—Tests
* `EntropyEntity`—Random number generator
* `DNetSrv`—Driver for working with network cards
* `VfsSdCardFs`—Program that supports the SD Card file system
* `VfsNet`—Program that is used for working with the network
* `Dhcpcd`—DHCP client implementation program that gets network interface parameters from an external
DHCP server in the background and passes them to the virtual file system
* `SDCard`—SD Card driver
* `BSP`—Driver for configuring pin multiplexing parameters (pinmux)
* `Bcm2711MboxArmToVc`—Mailbox driver for Raspberry Pi 4 B

### Initialization description

<details><summary>Statically created IPC channels</summary>

* `amqp.TestEntity` → `kl.VfsSdCardFs`
* `amqp.Consumer` → `kl.VfsNet`
* `kl.VfsNet` → `kl.EntropyEntity`
* `kl.VfsNet` → `kl.drivers.DNetSrv`
* `kl.rump.Dhcpcd` → `kl.VfsSdCardFs`
* `kl.rump.Dhcpcd` → `kl.VfsNet`
* `kl.VfsSdCardFs` → `kl.drivers.SDCard`
* `kl.VfsSdCardFs` → `kl.EntropyEntity`
* `kl.drivers.DNetSrv` → `kl.drivers.Bcm2711MboxArmToVc`
* `kl.drivers.SDCard` → `kl.drivers.BSP`

</details>

The [`./einit/src/init.yaml.in`](einit/src/init.yaml.in) template is used to automatically generate
part of the solution initialization description file `init.yaml`. For more information about the
`init.yaml.in` template file, see the
[KasperskyOS Community Edition Online Help](https://click.kaspersky.com/?hl=en-us&link=online_help&pid=kos&version=1.3&customization=KCE&helpid=cmake_yaml_templates).

### Security policy description

The [`./einit/src/security.psl.in`](einit/src/security.psl.in) template is used to automatically
generate part of the `security.psl` file using CMake tools. The `security.psl` file contains part of
a solution security policy description. For more information about the `security.psl` file, see
[Describing a security policy for a KasperskyOS-based solution](https://click.kaspersky.com/?hl=en-us&link=online_help&pid=kos&version=1.3&customization=KCE&helpid=ssp_descr).

[⬆ Back to Top](#Table-of-contents)

## Getting started

### Building and running the tests

The tests for KasperskyOS are built using the CMake build system, which is provided in the KasperskyOS
Community Edition SDK.

The `SDK_PREFIX` environment variables affects the build of the tests. It specifies the path to
the installed version of the KasperskyOS Community Edition SDK.

Run the following command `./cross-build.sh <TARGET> [SDK_PATH]`, where:

* `TARGET` can take one of the following values: `qemu` for QEMU or `hw` for Raspberry Pi 4 B or Radxa Rock 3A.
* `SDK_PATH` specifies the path to the installed version of the KasperskyOS Community Edition SDK.
If not specified, the path defined in the `SDK_PREFIX` environment variable is used. The value
specified in the `SDK_PATH` option takes precedence over the value of the `SDK_PREFIX` environment variable.

#### QEMU

Running `cross-build.sh` creates a KasperskyOS-based solution image that includes the tests.
The `kos-qemu-image` solution image is located in the `./build/einit` directory.

The `cross-build.sh` script both builds the tests on QEMU and runs them.

#### Hardware

Running `cross-build.sh` creates a KasperskyOS-based solution image that includes the tests and
a bootable SD card image for the hardware platform. The `kos-image` solution image is located in the
`./build/einit` directory. The `hdd.img` bootable SD card image is located in the `./build`
directory.

1. To copy the bootable SD card image to the SD card, connect the SD card to the computer and
run the following command:

   `$ sudo dd bs=64k if=build/hdd.img of=/dev/sd[X] conv=fsync`,

   where `[X]` is the final character in the name of the SD card block device.

1. Connect the bootable SD card to the hardware.
1. Supply power to the hardware and wait for the example to run.

You can also use an alternative option to prepare and run the tests:

1. Prepare the required hardware platform and bootable SD card by following the instructions in the KasperskyOS Community Edition Online Help:
    * [Raspberry Pi 4 B](https://click.kaspersky.com/?hl=en-us&link=online_help&pid=kos&version=1.3&customization=KCE&helpid=preparing_sd_card_rpi)
    * [Radxa ROCK 3A](https://click.kaspersky.com/?hl=en-us&link=online_help&pid=kos&version=1.3&customization=KCE&helpid=preparing_sd_card_radxa)
1. Run the tests by following the instructions in the
[KasperskyOS Community Edition Online Help](https://click.kaspersky.com/?hl=en-us&link=online_help&pid=kos&version=1.3&customization=KCE&helpid=running_sample_programs_rpi)

#### CMake input files

[./consumer/CMakeLists.txt](tests/CMakeLists.txt)—CMake commands for building the `TestEntity` program.

[./einit/CMakeLists.txt](einit/CMakeLists.txt)—CMake commands for building the `Einit` program and
the solution image.

[./CMakeLists.txt](CMakeLists.txt)—CMake commands for building the solution.

[⬆ Back to Top](#Table-of-contents)

© 2025 AO Kaspersky Lab
