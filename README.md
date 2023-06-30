# KasperskyOS modification of RabbitMQ-C AMQP client library

>This version of the [RabbitMQ® C AMQP client library](https://github.com/alanxz/rabbitmq-c) is adapted for KasperskyOS. The project is based on a [commit](https://github.com/alanxz/rabbitmq-c/commit/974d71adceae6d742ae20a4c880d99c131f1460a) of version 0.13.0 and includes two examples: an AMQP consumer and an AMQP publisher.

## What is a RabbitMQ-C AMQP client library for KasperskyOS?

The RabbitMQ-C AMQP client library for KasperskyOS provides the ability to send and receive messages between programs using a RabbitMQ message broker as an intermediary. For more information about RabbitMQ, see [RabbitMQ](https://www.rabbitmq.com/).

Communication between the RabbitMQ-C AMQP library and the RabbitMQ message broker is based on an Advanced Message Queuing Protocol (AMQP). For more information about AMQP, see [AMQP version 0.9.1](https://www.amqp.org/specification/0-9-1/amqp-org-download).

For more information about KasperskyOS, please refer to the [KasperskyOS Community Edition Online Help](https://support.kaspersky.com/help/KCE/1.1/en-US/community_edition.htm).

The RabbitMQ-C AMQP client library has a limitation. This implementation supports only AMQP version 0.9.1.

Other limitations and known issues are described in the [KasperskyOS Community Edition Online Help](https://support.kaspersky.ru/help/KCE/1.1/en-US/limitations_and_known_problems.htm).

## Table of contents

- [RabbitMQ-C AMQP client library for KasperskyOS](#rabbitmq-c-amqp-client-library-for-kasperskyos)
  - [What is a RabbitMQ-C AMQP client library for KasperskyOS?](#what-is-a-rabbitmq-c-amqp-client-library-for-kasperskyos)
  - [Table of contents](#table-of-contents)
  - [Getting started](#getting-started)
    - [Prerequisites](#prerequisites)
  - [Usage](#usage)
    - [Examples](#examples)
  - [Contributing](#contributing)
  - [Licensing](#licensing)

## Getting started

### Prerequisites

1. [Install](https://support.kaspersky.com/help/KCE/1.1/en-US/sdk_install_and_remove.htm) the KasperskyOS Community Edition SDK. You can download the latest version of the KasperskyOS Community Edition for free from [os.kaspersky.com](https://os.kaspersky.com/development/). Minimum required version of the KasperskyOS Community Edition SDK is 1.1.1.13. For more information, see [System requirements](https://support.kaspersky.com/help/KCE/1.1/en-US/system_requirements.htm).
1. Copy project sources files to your home directory. Examples of KasperskyOS-based solutions are located in the following directory:
   ```
   ./kos
   ```

[⬆ Back to Top](#Table-of-contents)

## Usage

When you develop a KasperskyOS-based solution, use the [recommended structure of project directories](https://support.kaspersky.com/help/KCE/1.1/en-US/cmake_using_sdk_cmake.htm) to simplify usage of CMake scripts.

To include the RabbitMQ-C AMQP client library in your KasperskyOS-based solution, follow these steps:

1. Build the library using the CMake module `ExternalProject`.
1. Use CMake command `target_include_directories()` to specify the paths to the header file directories.
1. Link the built library to your program using CMake command `target_link_libraries()`. Examples of implementing this approach are shown in the files: [`./kos/amqp_consumer/consumer/CMakeLists.txt`](kos/amqp_consumer/consumer/CMakeLists.txt) and [`./kos/amqp_publisher/publisher/CMakeLists.txt`](kos/amqp_publisher/publisher/CMakeLists.txt).
1. Add the required header files to your code file to enable calls to library functions:
   ```
   #include <amqp.h>
   #include <amqp_framing.h>
   #include <amqp_tcp_socket.h>
   ...
   ```

### Examples

* [`./kos/amqp_consumer`](kos/amqp_consumer)—Example of developing a solution that is an implementation of the AMQP consumer.
* [`./kos/amqp_publisher`](kos/amqp_publisher)—Example of developing a solution that is an implementation of the AMQP publisher.

[⬆ Back to Top](#Table-of-contents)

## Trademarks

RabbitMQ® is a trademark of VMware, Inc. in the U.S. and other countries.

## Contributing

Only KasperskyOS-specific changes can be approved. See [CONTRIBUTING.md](CONTRIBUTING.md) for detailed instructions on code contribution.

[⬆ Back to Top](#Table-of-contents)

## Licensing

This project is licensed under the terms of the MIT license. See [LICENSE](LICENSE) for more information.

[⬆ Back to Top](#Table-of-contents)
