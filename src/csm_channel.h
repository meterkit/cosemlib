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

#define INVALID_CHANNEL_ID 0U


typedef struct
{
    csm_request request;
    csm_asso_state *asso;   //!< Association used for that channel

} csm_channel;


void csm_channel_init(csm_channel *channel);
void csm_channel_disconnect(csm_channel *channel);
int csm_channel_execute(csm_request *request, csm_asso_state *asso, csm_array *packet);


#endif // CSM_CHANNEL_H
