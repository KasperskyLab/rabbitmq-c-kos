/*
 * © 2023 AO Kaspersky Lab. All Rights Reserved
 * Licensed under the MIT License
 */

#ifndef _AMQP_PUBLISHER_H
#define _AMQP_PUBLISHER_H

#include <rabbitmq-c/amqp.h>
#include <rabbitmq-c/tcp_socket.h>
#include <rabbitmq-c/framing.h>

class Publisher
{
private:
    char const             *m_exchange   = "amq.direct";
    char const             *m_routingKey = "test";
    int                     m_status;
    amqp_socket_t          *m_socket = NULL;
    amqp_connection_state_t m_conn;
    int                     m_chanel = 1;

public:
    Publisher(const char *host, int port);
    ~Publisher();

    void SendData();
};

#endif // _AMQP_PUBLISHER_H
