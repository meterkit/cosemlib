#ifndef HDLC_H
#define HDLC_H

#include "transports.h"

typedef struct
{
    uint16_t max_rcv_info_size;
    uint16_t max_snd_info_size;
    uint16_t address;
    data_handler handler;

} hdlc_channel;


void hdlc_init(hdlc_channel *chan);
int hdlc_handle(hdlc_channel *chan, uint8_t *data, uint16_t size);


#endif // HDLC_H
