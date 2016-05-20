#ifndef TRANSPORTS_H
#define TRANSPORTS_H

#include <stdint.h>

/**
 * @brief Cosem handler called from the transport layer upon reception of data
 */
typedef int (*data_handler)(uint8_t channel, uint8_t *buffer, size_t size);

enum conn_event
{
    CONN_DISCONNECTED,
    CONN_NEW
};

/**
 * @brief Connection handler called from the transport
 * @return 0 if connection is rejected, otherwise returns the channel identifier
 */
typedef uint8_t (*conn_handler)(uint8_t channel, enum conn_event event);

#endif // TRANSPORTS_H

