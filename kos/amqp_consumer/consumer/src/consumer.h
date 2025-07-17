/*
 * © 2023 AO Kaspersky Lab
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
    amqp_bytes_t            m_queueName  = {};
    amqp_connection_state_t m_conn;

public:
    Consumer(const char *host, int port);
    ~Consumer();

    void DeclareQueue();
    void RecvAndPrintData();
};

#endif // _AMQP_CONSUMER_H
