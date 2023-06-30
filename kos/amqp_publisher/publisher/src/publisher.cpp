/*
 * © 2023 AO Kaspersky Lab. All Rights Reserved
 * Licensed under the MIT License
 */

#include <iostream>
#include <string>

#include <utils.hpp>

#include "general.h"
#include "publisher.h"

Publisher::Publisher(const char *hostname, int port)
{
    m_conn = amqp_new_connection();
    if (!m_conn)
    {
        throw std::runtime_error("Allocation error for connection object");
    }

    m_socket = amqp_tcp_socket_new(m_conn);
    if (!m_socket)
    {
        throw std::runtime_error("Error creating TCP socket");
    }

    m_status = amqp_socket_open(m_socket, hostname, port);
    if (m_status)
    {
        throw std::runtime_error("Error opening TCP socket");
    }

    utils::ThrowOnAmqpError(
        amqp_login(m_conn, "/", 0, 131072, 0, AMQP_SASL_METHOD_PLAIN, "guest", "guest"),
        "Logging in"
    );

    amqp_channel_open(m_conn, m_chanel);

    utils::ThrowOnAmqpError(amqp_get_rpc_reply(m_conn), "Opening channel");
}

Publisher::~Publisher()
{
    amqp_channel_close(m_conn, m_chanel, AMQP_REPLY_SUCCESS);
    amqp_connection_close(m_conn, AMQP_REPLY_SUCCESS);
    amqp_destroy_connection(m_conn);
}

void Publisher::SendData()
{
    amqp_basic_properties_t props;
    props._flags        = AMQP_BASIC_CONTENT_TYPE_FLAG | AMQP_BASIC_DELIVERY_MODE_FLAG;
    props.content_type  = amqp_cstring_bytes("text/plain");
    props.delivery_mode = 2; /* persistent delivery mode */

    for (int i = 0; i < 100; i++)
    {
        std::string messageBody = "{\"sequence\"=" + std::to_string(i + 1) + "}";
        std::cout << app::AppTag << messageBody << "\n\r";

        utils::ThrowOnError(
            amqp_basic_publish(
                m_conn,
                m_chanel,
                amqp_cstring_bytes(m_exchange),
                amqp_cstring_bytes(m_routingKey),
                0,
                0,
                &props,
                amqp_cstring_bytes(messageBody.c_str())),
            "Publishing"
        );
    }
}
