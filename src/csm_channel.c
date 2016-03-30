#include "csm_channel.h"
#include "csm_config.h"
#include "csm_services.h"


void csm_channel_init(csm_channel *channel)
{
    channel->asso = NULL;
    channel->request.channel_id = INVALID_CHANNEL_ID;
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
            if (asso->state_cf == CF_ASSOCIATED)
            {
                ret = csm_services_execute(asso, request, packet);
            }
            else if (asso->state_cf == CF_ASSOCIATION_PENDING)
            {
                // In case of HLS, we have to access to one attribute
                ret = csm_services_hls_execute(asso, request, packet);
            }
            else
            {
                CSM_ERR("[CHANNEL] Association is not open");
            }
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
