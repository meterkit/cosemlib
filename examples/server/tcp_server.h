#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include <stdlib.h>
#include <stdint.h>

enum conn_event
{
    CONN_DISCONNECTED,
    CONN_NEW
};

/**
 * @brief Data handler called from the TCP server upon reception of data
 */
typedef int (*data_handler)(uint8_t channel, uint8_t *buffer, size_t size);

/**
 * @brief Connection handler called from the TCP server
 * @return 0 if connection is rejected, otherwise returns the channel identifier
 */
typedef uint8_t (*conn_handler)(uint8_t channel, enum conn_event event);

int tcp_server_init(data_handler data_func, conn_handler conn_func, char *buffer, int buf_size, int tcp_port);

#endif // TCP_SERVER_H
