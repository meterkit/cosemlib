#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include <stdlib.h>
#include <stdint.h>

// Callbacks from the TCP stack
typedef int (*data_handler)(uint8_t channel, char *buffer, size_t size);

int tcp_server_init(data_handler data_func, char *buffer, int buf_size);

#endif // TCP_SERVER_H
