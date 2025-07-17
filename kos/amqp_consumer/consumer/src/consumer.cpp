/*
 * © 2024 AO Kaspersky Lab
 * Licensed under the MIT License
 */

#include <ctime>
#include <cstring>
#include <iostream>

#include <utils.hpp>

#include "general.h"
#include "consumer.h"

Consumer::Consumer(const char *hostname, int port)
    : m_conn{amqp_new_connection()}
{
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
        std::string errMsg("Error opening broker connection socket with address: ");
        errMsg.append(hostname).append(" port: ").append(std::to_string(port));
        throw std::runtime_error(errMsg);
    }

    utils::ThrowOnAmqpError(
        amqp_login(m_conn, "/", 0, 131072, 0, AMQP_SASL_METHOD_PLAIN, "guest", "guest"),
        "Logging in"
    );

    amqp_channel_open(m_conn, m_chanel);

    utils::ThrowOnAmqpError(amqp_get_rpc_reply(m_conn), "Opening channel");
}

Consumer::~Consumer()
{
    amqp_bytes_free(m_queueName);
    amqp_channel_close(m_conn, m_chanel, AMQP_REPLY_SUCCESS);
    amqp_connection_close(m_conn, AMQP_REPLY_SUCCESS);
}

void Consumer::DeclareQueue()
{
    amqp_queue_declare_ok_t *r = amqp_queue_declare(m_conn, m_chanel, amqp_empty_bytes,
                                    0, 0, 0, 1, amqp_empty_table);

    utils::ThrowOnAmqpError(amqp_get_rpc_reply(m_conn), "Declaring queue");

    m_queueName = amqp_bytes_malloc_dup(r->queue);
    if (!m_queueName.bytes)
    {
        throw std::runtime_error("Out of memory while copying queue name");
    }

    amqp_queue_bind(
        m_conn,
        m_chanel,
        m_queueName,
        amqp_cstring_bytes(m_exchange),
        amqp_cstring_bytes(m_bindingKey),
        amqp_empty_table
    );

    utils::ThrowOnAmqpError(amqp_get_rpc_reply(m_conn), "Binding queue");

    amqp_basic_consume(m_conn, m_chanel, m_queueName, amqp_empty_bytes, 0, 1, 0,
                        amqp_empty_table);

    utils::ThrowOnAmqpError(amqp_get_rpc_reply(m_conn), "Consuming");
}

void Consumer::RecvAndPrintData()
{
    while (true)
    {
        amqp_rpc_reply_t res;
        amqp_envelope_t  envelope;

        amqp_maybe_release_buffers(m_conn);

        res = amqp_consume_message(m_conn, &envelope, NULL, 0);
        if (AMQP_RESPONSE_NORMAL != res.reply_type)
        {
            break;
        }

        std::string exchBytes{
            static_cast<char*>(envelope.exchange.bytes),
            envelope.exchange.len
        };

        std::string rKeyBytes{
            static_cast<char*>(envelope.routing_key.bytes),
            envelope.routing_key.len
        };

        std::cout << "Delivery " <<  envelope.delivery_tag << ", exchange: "
                  << exchBytes << ", routingkey: " << rKeyBytes << std::endl;

        if (envelope.message.properties._flags & AMQP_BASIC_CONTENT_TYPE_FLAG)
        {
            std::string contentBytes{
                static_cast<char*>(envelope.message.properties.content_type.bytes),
                envelope.message.properties.content_type.len
            };

            std::cout << "Content-type: " << contentBytes << std::endl;
        }

        std::cout << "----" << std::endl;

        utils::AmqpDump(envelope.message.body.bytes, envelope.message.body.len);
        amqp_destroy_envelope(&envelope);
    }
}
