/*
 * © 2023 AO Kaspersky Lab
 * Licensed under the MIT License
 */

#include <string>
#include <iostream>
#include <iomanip>
#include <cstring>
#include <sstream>

#include <sys/time.h>

#include <rabbitmq-c/amqp.h>
#include <rabbitmq-c/framing.h>

#include "utils.hpp"

namespace utils {

uint64_t MicroSecNow(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (uint64_t)tv.tv_sec * 1000000 + (uint64_t)tv.tv_usec;
}

void MicroSleep(int usec)
{
    struct timespec req;
    req.tv_sec  = 0;
    req.tv_nsec = 1000 * usec;
    nanosleep(&req, NULL);
}

void ThrowOnError(int x, std::string context)
{
    if (x < 0)
    {
        throw std::runtime_error(context + ": " + amqp_error_string2(x));
    }
}

void ThrowOnAmqpError(amqp_rpc_reply_t x, std::string context)
{
    switch (x.reply_type)
    {
        case AMQP_RESPONSE_NORMAL:
            break;

        case AMQP_RESPONSE_NONE:
            throw std::runtime_error(context + ": missing RPC reply type!");

        case AMQP_RESPONSE_LIBRARY_EXCEPTION:
            throw std::runtime_error(context + ":" + amqp_error_string2(x.library_error));

        case AMQP_RESPONSE_SERVER_EXCEPTION:
            switch (x.reply.id)
            {
                case AMQP_CONNECTION_CLOSE_METHOD:
                {
                    amqp_connection_close_t *m = (amqp_connection_close_t *)x.reply.decoded;

                    throw std::runtime_error(context + ": server connection error " +
                          std::to_string(m->reply_code) + "h, message: " +
                          std::string{
                            static_cast<char *>(m->reply_text.bytes),
                            m->reply_text.len
                          }
                    );
                }
                case AMQP_CHANNEL_CLOSE_METHOD:
                {
                    amqp_channel_close_t *m = (amqp_channel_close_t *)x.reply.decoded;
                    throw std::runtime_error(context + ": server channel error " +
                          std::to_string(m->reply_code) + "h, message: " +
                          std::string{
                            static_cast<char *>(m->reply_text.bytes),
                            m->reply_text.len
                          }
                    );
                }
                default:
                    std::stringstream msg;
                    msg << context << ": unknown server error, method id 0x"
                        << std::hex << std::setw(8) << std::setfill('0') << x.reply.id;
                    throw std::runtime_error(msg.str());
            }
            break;
    }
}

static void DumpRow(long count, long numinrow, int *chs)
{
    int i;

    std::cout << std::hex << std::setw(8) << std::setfill('0') << count - numinrow;

    if (numinrow > 0)
    {
        for (i = 0; i < numinrow; i++)
        {
            if (i == 8)
            {
                std::cout << " :";
            }
            std::cout << std::hex << std::setw(2) << std::setfill('0') << chs[i];
        }

        for (i = numinrow; i < 16; i++)
        {
            if (i == 8)
            {
                std::cout << " :";
            }
            std::cout << "   ";
        }

        std::cout << "  ";

        for (i = 0; i < numinrow; i++)
        {
            if (isprint(chs[i]))
            {
                std::cout << char(chs[i]);
            }
            else
            {
                std::cout << ".";
            }
        }
    }

    std::cout << std::endl;
}

static bool RowsEq(int *a, int *b)
{
    int i;

    for (i = 0; i < 16; i++)
        if (a[i] != b[i])
        {
            return false;
        }

    return true;
}

void AmqpDump(void const *buffer, size_t len)
{
    uint8_t *buf = (uint8_t *)buffer;

    long   count    = 0;
    int    numinrow = 0;
    int    chs[16];
    int    oldchs[16]  = {0};
    int    showed_dots = 0;
    size_t i;

    for (i = 0; i < len; i++)
    {
        int ch = buf[i];

        if (numinrow == 16)
        {
            int j;

            if (RowsEq(oldchs, chs))
            {
                if (!showed_dots)
                {
                    showed_dots = 1;
                    std::cout <<
                        "          .. .. .. .. .. .. .. .. : .. .. .. .. .. .. .. .."
                              << std::endl;
                }
            }
            else
            {
                showed_dots = 0;
                DumpRow(count, numinrow, chs);
            }

            memccpy(oldchs, chs, 16, sizeof(int));
            numinrow = 0;
        }

        count++;
        chs[numinrow++] = ch;
    }

    DumpRow(count, numinrow, chs);

    if (numinrow != 0)
    {
        std::cout << std::hex << std::setw(8) << std::setfill('0') << count << std::endl;
    }

    std::cout << std::resetiosflags(std::ios_base::basefield);
}

} // namespace utils
