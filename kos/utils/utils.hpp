/*
 * Copyright 2007 - 2021, Alan Antonuk and the rabbitmq-c contributors.
 * SPDX-License-Identifier: mit
 * © 2023 AO Kaspersky Lab
 */

#ifndef _UTILS_H
#define _UTILS_H

#include <string>
#include <rabbitmq-c/amqp.h>

namespace utils {
void     ThrowOnError(int x, std::string context);
void     ThrowOnAmqpError(amqp_rpc_reply_t x, std::string context);
void     AmqpDump(void const *buffer, size_t len);
void     MicroSleep(int usec);
uint64_t MicroSecNow(void);
} // namespace utils

#endif // _UTILS_H
