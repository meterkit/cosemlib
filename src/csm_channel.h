#ifndef CSM_CHANNEL_H
#define CSM_CHANNEL_H

#include "csm_association.h"
#include "csm_services.h"

/**
 * @brief All data and states for one request
 */
typedef struct
{
    uint8_t ssap;

} csm_context;

typedef struct
{
    uint8_t channel;        //< Physical channel - set one bit only
    csm_context context;

} csm_channel;


int csm_channel_init(csm_channel *channel);
int csm_channel_execute(csm_channel *channel, csm_asso_state *asso, csm_llc *llc, csm_array *packet);


#endif // CSM_CHANNEL_H
