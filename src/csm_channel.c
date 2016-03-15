#include "csm_channel.h"
#include "csm_config.h"
#include "csm_services.h"


void csm_channel_init(csm_channel *channel)
{
    channel->asso = NULL;
    channel->id = INVALID_CHANNEL_ID;
}

int csm_channel_execute(csm_request *request, csm_asso_state *asso, csm_array *packet)
{
    int ret = FALSE;

    uint8_t tag;
    if (csm_array_get(packet, 0U, &tag))
    {
        switch (tag)
        {
        case CSM_ASSO_AARE:
        case CSM_ASSO_AARQ:
        case CSM_ASSO_RLRE:
        case CSM_ASSO_RLRQ:
            ret = csm_asso_execute(asso, packet);
            break;
        default:
            ret = csm_services_execute(asso, request, packet);
            break;
        }
    }
    return ret;
}

void csm_channel_disconnect(csm_channel *channel)
{
    if (channel->asso != NULL)
    {
        channel->asso->state_cf = CF_IDLE;
    }
}
