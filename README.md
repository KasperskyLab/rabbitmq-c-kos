# KasperskyOS modification of RabbitMQ-C AMQP client library

This version of the [RabbitMQ® C AMQP client library](https://github.com/alanxz/rabbitmq-c) is
adapted for KasperskyOS. The project is based on a [commit](https://github.com/alanxz/rabbitmq-c/commit/974d71adceae6d742ae20a4c880d99c131f1460a)
of version 0.13.0 and includes two examples: an AMQP consumer and an AMQP publisher.

The RabbitMQ-C AMQP client library for KasperskyOS provides the ability to send and receive messages
between programs using a RabbitMQ message broker as an intermediary. For more information about
RabbitMQ, see [RabbitMQ](https://www.rabbitmq.com/).

Communication between the RabbitMQ-C AMQP library and the RabbitMQ message broker is based on
an Advanced Message Queuing Protocol (AMQP). For more information about AMQP, see
[AMQP version 0.9.1](https://www.rabbitmq.com/tutorials/amqp-concepts).

For more information about KasperskyOS, please refer to the
[KasperskyOS Community Edition Online Help](https://click.kaspersky.com/?hl=en-us&link=online_help&pid=kos&version=1.2&customization=KCE_community_edition).

This implementation of RabbitMQ-C AMQP client library only supports AMQP version 0.9.1.

Other limitations and known issues are described in the
[KasperskyOS Community Edition Online Help](https://click.kaspersky.com/?hl=en-us&link=online_help&pid=kos&version=1.2&customization=KCE_limitations_and_known_problems).

## Table of contents
- [KasperskyOS modification of RabbitMQ-C AMQP client library](#kasperskyos-modification-of-rabbitmq-c-amqp-client-library)
  - [Table of contents](#table-of-contents)
  - [Getting started](#getting-started)
    - [Prerequisites](#prerequisites)
  - [Usage](#usage)
    - [Examples](#examples)
    - [Tests](#tests)
  - [Trademarks](#trademarks)
  - [Contributing](#contributing)
  - [Licensing](#licensing)

## Getting started

### Prerequisites

1. [Install](https://click.kaspersky.com/?hl=en-us&link=online_help&pid=kos&version=1.2&customization=KCE_sdk_install_and_remove)
KasperskyOS Community Edition SDK. You can download the latest version of the KasperskyOS Community
Edition for free from [os.kaspersky.com](https://os.kaspersky.com/development/). The minimum required
version of KasperskyOS Community Edition SDK is 1.2. For more information, see
[System requirements](https://click.kaspersky.com/?hl=en-us&link=online_help&pid=kos&version=1.2&customization=KCE_system_requirements).
1. Copy project source files to your home directory. Examples of KasperskyOS-based solutions are located in the [`./kos`](kos) directory.

## Usage

When you develop a KasperskyOS-based solution, use the
[recommended structure of project directories](https://click.kaspersky.com/?hl=en-us&link=online_help&pid=kos&version=1.2&customization=KCE_cmake_using_sdk_cmake)
to simplify usage of CMake scripts.

To include the RabbitMQ-C AMQP client library in your KasperskyOS-based solution, follow these steps:

1. Build the library using the CMake module `ExternalProject`.
1. Use CMake command `target_include_directories()` to specify the paths to the header file directories.
1. Link the built library to your program using CMake command `target_link_libraries()`. Examples of
implementing this approach are shown in the files: [`./kos/amqp_consumer/consumer/CMakeLists.txt`](kos/amqp_consumer/consumer/CMakeLists.txt)
and [`./kos/amqp_publisher/publisher/CMakeLists.txt`](kos/amqp_publisher/publisher/CMakeLists.txt).
1. Add the required header files to your code file to enable calls to library functions:
   ```
   #include <amqp.h>
   #include <amqp_framing.h>
   #include <amqp_tcp_socket.h>
   ...
   ```

This solution uses the dynamic variant of the RabbitMQ-C AMQP client library. To build a static variant
of the library, use the `initialize_platform` command with the `FORCE_STATIC` parameter. For additional
details regarding this command, please refer to the
[platform library](https://click.kaspersky.com/?hl=en-us&link=online_help&pid=kos&version=1.2&customization=KCE_cmake_platform_lib).

### Examples

* [`./kos/amqp_consumer`](kos/amqp_consumer)—Example of developing a solution that is an implementation
of the AMQP consumer.
* [`./kos/amqp_publisher`](kos/amqp_publisher)—Example of developing a solution that is an
implementation of the AMQP publisher.

### Tests

[`./kos/unit_tests`](kos/unit_tests)—Unit testing using the GoogleTest framework.

## Trademarks

Registered trademarks and endpoint marks are the property of their respective owners.

GoogleTest is a trademark of Google LLC.

RabbitMQ® is a trademark of VMware, Inc. in the U.S. and other countries.

Raspberry Pi is a trademark of the Raspberry Pi Foundation.

## Contributing

Only KasperskyOS-specific changes can be approved. See [CONTRIBUTING.md](CONTRIBUTING.md)
for detailed instructions on code contribution.

## Licensing

This project is licensed under the terms of the MIT license. See [LICENSE](LICENSE) for more information.

[⬆ Back to Top](#Table-of-contents)

© 2024 AO Kaspersky Lab
