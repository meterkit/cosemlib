/**
 * Copyright (c) 2016, Anthony Rabine
 * See LICENSE.txt
 *
 * BER decoder/encoder/helper
 */

#ifndef CSM_CHANNEL_H
#define CSM_CHANNEL_H

#include "csm_association.h"
#include "csm_services.h"


typedef struct
{
    uint8_t channel;        //< Physical channel - set one bit only
    csm_request request;

} csm_channel;


int csm_channel_init(csm_channel *channel);
int csm_channel_execute(csm_channel *channel, csm_asso_state *asso, csm_array *packet);


#endif // CSM_CHANNEL_H
