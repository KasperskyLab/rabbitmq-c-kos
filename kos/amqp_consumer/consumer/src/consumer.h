/*
 * © 2023 AO Kaspersky Lab. All Rights Reserved
 * Licensed under the MIT License
 */

#ifndef _AMQP_CONSUMER_H
#define _AMQP_CONSUMER_H

#include <rabbitmq-c/amqp.h>
#include <rabbitmq-c/tcp_socket.h>
#include <rabbitmq-c/framing.h>

class Consumer
{
private:
    int                     m_status;
    char const             *m_exchange   = "amq.direct";
    char const             *m_bindingKey = "test";
    int                     m_chanel     = 1;
    amqp_socket_t          *m_socket     = NULL;
    amqp_connection_state_t m_conn;
    amqp_bytes_t            m_queueName;

public:
    Consumer(const char *host, int port);
    ~Consumer();

    void DeclareQueue();
    void RecvAndPrintData();
};

#endif // _AMQP_CONSUMER_H
