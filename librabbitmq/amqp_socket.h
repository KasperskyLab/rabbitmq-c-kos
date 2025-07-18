// Copyright 2007 - 2021, Alan Antonuk and the rabbitmq-c contributors.
// SPDX-License-Identifier: mit
//
// © 2023 AO Kaspersky Lab
// Licensed under the MIT License

/**
 * An abstract socket interface.
 */

#ifndef AMQP_SOCKET_H
#define AMQP_SOCKET_H

#include "amqp_private.h"
#include "amqp_time.h"

AMQP_BEGIN_DECLS

typedef enum {
  AMQP_SF_NONE = 0,
  AMQP_SF_MORE = 1,
  AMQP_SF_POLLIN = 2,
  AMQP_SF_POLLOUT = 4,
  AMQP_SF_POLLERR = 8
} amqp_socket_flag_enum;

typedef enum { AMQP_SC_NONE = 0, AMQP_SC_FORCE = 1 } amqp_socket_close_enum;

int amqp_os_socket_error(void);

int amqp_os_socket_close(int sockfd);

/* Socket callbacks. */
typedef ssize_t (*amqp_socket_send_fn)(void *, const void *, size_t, int);
typedef ssize_t (*amqp_socket_recv_fn)(void *, void *, size_t, int);
typedef int (*amqp_socket_open_fn)(void *, const char *, int,
                                   const struct timeval *);
typedef int (*amqp_socket_close_fn)(void *, amqp_socket_close_enum);
typedef int (*amqp_socket_get_sockfd_fn)(void *);
typedef void (*amqp_socket_delete_fn)(void *);

/** V-table for amqp_socket_t */
struct amqp_socket_class_t {
  amqp_socket_send_fn send;
  amqp_socket_recv_fn recv;
  amqp_socket_open_fn open;
  amqp_socket_close_fn close;
  amqp_socket_get_sockfd_fn get_sockfd;
  amqp_socket_delete_fn delete_sock;
};

/** Abstract base class for amqp_socket_t */
struct amqp_socket_t_ {
  const struct amqp_socket_class_t *klass;
};

/**
 * Set set the socket object for a connection
 *
 * This assigns a socket object to the connection, closing and deleting any
 * existing socket
 *
 * \param [in] state The connection object to add the socket to
 * \param [in] socket The socket object to assign to the connection
 */
void amqp_set_socket(amqp_connection_state_t state, amqp_socket_t *socket);

/**
 * Send a message from a socket.
 *
 * This function wraps send(2) functionality.
 *
 * This function will only return on error, or when all of the bytes in buf
 * have been sent, or when an error occurs.
 *
 * \param [in,out] self A socket object.
 * \param [in] buf A buffer to read from.
 * \param [in] len The number of bytes in \e buf.
 * \param [in]
 *
 * \return AMQP_STATUS_OK on success. amqp_status_enum value otherwise
 */
ssize_t amqp_socket_send(amqp_socket_t *self, const void *buf, size_t len,
                         int flags);

ssize_t amqp_try_send(amqp_connection_state_t state, const void *buf,
                      size_t len, amqp_time_t deadline, int flags);

/**
 * Receive a message from a socket.
 *
 * This function wraps recv(2) functionality.
 *
 * \param [in,out] self A socket object.
 * \param [out] buf A buffer to write to.
 * \param [in] len The number of bytes at \e buf.
 * \param [in] flags Receive flags, implementation specific.
 *
 * \return The number of bytes received, or < 0 on error (\ref amqp_status_enum)
 */
ssize_t amqp_socket_recv(amqp_socket_t *self, void *buf, size_t len, int flags);

/**
 * Close a socket connection and free resources.
 *
 * This function closes a socket connection and releases any resources used by
 * the object. After calling this function the specified socket should no
 * longer be referenced.
 *
 * \param [in,out] self A socket object.
 * \param [in] force, if set, just close the socket, don't attempt a TLS
 * shutdown.
 *
 * \return Zero upon success, non-zero otherwise.
 */
int amqp_socket_close(amqp_socket_t *self, amqp_socket_close_enum force);

/**
 * Destroy a socket object
 *
 * \param [in] self the socket object to delete
 */
void amqp_socket_delete(amqp_socket_t *self);

/**
 * Open a socket connection.
 *
 * This function opens a socket connection returned from amqp_tcp_socket_new()
 * or amqp_ssl_socket_new(). This function should be called after setting
 * socket options and prior to assigning the socket to an AMQP connection with
 * amqp_set_socket().
 *
 * \param [in] host Connect to this host.
 * \param [in] port Connect on this remote port.
 * \param [in] timeout Max allowed time to spent on opening. If NULL - run in
 * blocking mode
 *
 * \return File descriptor upon success, non-zero negative error code otherwise.
 */
int amqp_open_socket_noblock(char const *hostname, int portnumber,
                             const struct timeval *timeout);

int amqp_open_socket_inner(char const *hostname, int portnumber,
                           amqp_time_t deadline);

/* Wait up to dealline for fd to become readable or writeable depending on
 * event (AMQP_SF_POLLIN, AMQP_SF_POLLOUT) */
int amqp_poll(int fd, int event, amqp_time_t deadline);

int amqp_send_method_inner(amqp_connection_state_t state,
                           amqp_channel_t channel, amqp_method_number_t id,
                           void *decoded, int flags, amqp_time_t deadline);

int amqp_queue_frame(amqp_connection_state_t state, amqp_frame_t *frame);

int amqp_put_back_frame(amqp_connection_state_t state, amqp_frame_t *frame);

int amqp_simple_wait_frame_on_channel(amqp_connection_state_t state,
                                      amqp_channel_t channel,
                                      amqp_frame_t *decoded_frame);

int sasl_mechanism_in_list(amqp_bytes_t mechanisms,
                           amqp_sasl_method_enum method);

int amqp_merge_capabilities(const amqp_table_t *base, const amqp_table_t *add,
                            amqp_table_t *result, amqp_pool_t *pool);
AMQP_END_DECLS

#endif /* AMQP_SOCKET_H */
