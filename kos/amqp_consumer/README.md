# AMQP consumer example

> An example of developing a KasperskyOS-based solution that is an implementation of an AMQP consumer.

## Table of contents
- [AMQP consumer example](#amqp-consumer-example)
  - [Table of contents](#table-of-contents)
  - [Solution overview](#solution-overview)
    - [List of programs](#list-of-programs)
    - [Solution scheme](#solution-scheme)
    - [Initialization description](#initialization-description)
    - [Security policy description](#security-policy-description)
  - [Getting started](#getting-started)
    - [Prerequisites](#prerequisites)
      - [QEMU](#qemu)
      - [Raspberry Pi 4 B](#raspberry-pi-4-b)
    - [Building and running the example](#building-and-running-the-example)
      - [QEMU](#qemu-1)
      - [Raspberry Pi 4 B](#raspberry-pi-4-b-1)
      - [CMake input files](#cmake-input-files)
  - [Usage](#usage)

## Solution overview

### List of programs

* `Consumer`—Program that is an implementation of the AMQP consumer
* `Ntpd`—Program that is an implementation of the NTP client that receives time parameters from external NTP servers in the background and passes them to the KasperskyOS kernel
* `VfsSdCardFs`—Program that supports the SDCard file system
* `VfsNet`—Program that is used for working with the network
* `Dhcpcd`—Program that is an implementation of the DHCP client that gets the parameters of network interfaces from an external DHCP server in the background and passes them to the virtual file system
* `SDCard`—SDCard driver
* `BSP`—Driver for configuring pin multiplexing parameters (pinmux)
* `GPIO`—GPIO support driver

[⬆ Back to Top](#Table-of-contents)

### Solution scheme

```mermaid
graph LR
  classDef Ksp fill:#006d5d, stroke:#006d5d, color:white;
  classDef SubKOS fill:#e8e8e8, stroke:#e8e8e8;
  classDef Hdd fill:#bfe9e3, stroke:#006d5d;
  subgraph Docker ["Docker container"]
  node1(RabbitMQ)
  end
  subgraph KOS ["KasperskyOS"]
    node2("AMQP consumer for KasperskyOS")
  end
  node3("Publisher<br>(amqp-tools)")
  node1-->|"AMQP"|node2
  node3-->|"AMQP"|node1
  class node2 Ksp
  class KOS,Docker SubKOS
```

[⬆ Back to Top](#Table-of-contents)

### Initialization description

 <details><summary>Statically created IPC channels</summary>

* `amqp.Consumer` → `kl.VfsSdCardFs`
* `amqp.Consumer` → `kl.VfsNet`
* `kl.Ntpd` → `kl.VfsSdCardFs`
* `kl.Ntpd` → `kl.VfsNet`
* `kl.VfsSdCardFs` → `kl.drivers.SDCard`
* `kl.VfsNet` → `kl.drivers.BSP`
* `kl.VfsNet` → `kl.drivers.GPIO`
* `kl.rump.Dhcpcd` → `kl.VfsSdCardFs`
* `kl.rump.Dhcpcd` → `kl.VfsNet`
* `kl.drivers.SDCard` → `kl.drivers.BSP`
</details>

The [`./einit/src/init.yaml.in`](einit/src/init.yaml.in) template is used to automatically generate a part of the solution initialization description file `init.yaml`. For more information about the `init.yaml.in` template file, see the [KasperskyOS Community Edition Online Help](https://support.kaspersky.com/help/KCE/1.1/en-US/cmake_yaml_templates.htm).

[⬆ Back to Top](#Table-of-contents)

### Security policy description

The [`./einit/src/security.psl`](einit/src/security.psl) file contains a solution security policy description. For more information about the `security.psl` file, see [Describing a security policy for a KasperskyOS-based solution](https://support.kaspersky.com/help/KCE/1.1/en-US/ssp_descr.htm).

[⬆ Back to Top](#Table-of-contents)

## Getting started

### Prerequisites

Make sure that you have installed the latest version of the [KasperskyOS Community Edition SDK](https://os.kaspersky.com/development/).

#### QEMU

1. Make sure that the Docker software is installed and running.
   ```
   $ systemctl status docker
   ```
1. To run the RabbitMQ message broker, run the [RabbitMQ Docker official image](https://hub.docker.com/_/rabbitmq) using the following command:
   ```
   $ docker run -d -p 5672:5672 --hostname my-rabbit --name some-rabbit rabbitmq:3
   ```
1. Create an alias for your network interface with the address `10.0.2.2/24`:
   ```
   $ sudo ip a a 10.0.2.2/24 dev docker0
   ```
   >A static IP address `10.0.2.2` and port `5672` are set for RabbitMQ message broker using the environment variables `AMQP_BROKER_ADDRESS` and `AMQP_BROKER_PORT`. You can change the broker address and port in the file [`./einit/src/init.yaml.in`](einit/src/init.yaml.in) according to your network configuration.
1. Install the package `amqp-tools` (command-line utilities for interacting with AMQP servers):
   ```
   $ sudo apt-get update -y
   $ sudo apt install amqp-tools
   ```
1. To check your environment, run the following command:
   ```
   $ docker stats
   ```
   The screen should display information about the running container named `some-rabbit`.

#### Raspberry Pi 4 B

1. To install required packages on your host system, run the following command:
   ```
   $ sudo apt install rabbitmq-server amqp-tools
   ```
1. Set your computer network interface to have a static IPv4 address `10.0.2.2/24`.
1. To make sure you have the RabbitMQ message broker running, run the following command:
   ```
   $ systemctl status rabbitmq-server.service
   ```

[⬆ Back to Top](#Table-of-contents)

### Building and running the example

The AMQP consumer for KasperskyOS is built using the CMake build system, which is provided in the KasperskyOS Community Edition SDK.

In this example, an AMQP publisher must be started on the host operating system, and the AMQP consumer must be started on KasperskyOS.

Go to the directory with the AMQP consumer example and run the following command `./cross-build.sh <platform> [SDK_PATH]`, where:

* `platform` can take one of the following values: `qemu` for QEMU or `rpi` for Raspberry Pi 4 B.
* `SDK_PATH` specifies the path to the installed version of the KasperskyOS Community Edition SDK. If not specified, the path `/opt/KasperskyOS-Community-Edition-1.1.1.13` is used by default.

For example, review the following command:
```
$ ./cross-build.sh qemu /opt/KasperskyOS-Community-Edition-1.1.1.13
```
The command builds the AMQP consumer example with the built-in RabbitMQ-C AMQP client library and runs the KasperskyOS-based solution image on QEMU. The solution image is based on the SDK found in the `/opt/KasperskyOS-Community-Edition-1.1.1.13` path.

#### QEMU

Running `cross-build.sh` creates a KasperskyOS-based solution image that includes the example. The `kos-qemu-image` solution image is located in the `./build/einit` directory.

The `cross-build.sh` script not only builds the example on QEMU, but also runs it.

#### Raspberry Pi 4 B

Running `cross-build.sh` creates a KasperskyOS-based solution image that includes the example. The `kos-image` solution image is located in the `./build/einit` directory.

1. Prepare Raspberry Pi 4 B and a bootable SD card to run the example by following the instructions in the [KasperskyOS Community Edition Online Help](https://support.kaspersky.com/help/KCE/1.1/en-US/preparing_sd_card_rpi.htm).
1. Run the example by following the instructions in the [KasperskyOS Community Edition Online Help](https://support.kaspersky.com/help/KCE/1.1/en-US/running_sample_programs_rpi.htm)

#### CMake input files

[./consumer/CMakeLists.txt](consumer/CMakeLists.txt)—CMake commands for building the `Consumer` program.

[./einit/CMakeLists.txt](einit/CMakeLists.txt)—CMake commands for building the `Einit` program and the solution image.

[./CMakeLists.txt](CMakeLists.txt)—CMake commands for building the solution.

[⬆ Back to Top](#Table-of-contents)

## Usage

After building and running the example, follow these steps:

1. Wait until a message similar to the following appears in the QEMU standard output:
   ```
   [Consumer] Waiting to receive data ...
   ```
1. To start the AMQP publisher on the host system, run the following command:
   ```
   $ for count in {1..100}; do amqp-publish --server=localhost --port=5672 --exchange=amq.direct --routing-key=test --body="{\"sequence\"=$count}"; done
   ```
1. A message similar to the following should appear in the QEMU standard output:
   ```
   Delivery 1, exchange: amq.direct, routingkey: test
   ----
   000000007b2273657175656e :6365223d317d        {"sequence"=1}
   0000000e

   ...

   Delivery 100, exchange: amq.direct, routingkey: test
   ----
   000000007b2273657175656e :6365223d3130307d  {"sequence"=100}
   00000010
   ```

[⬆ Back to Top](#Table-of-contents)
