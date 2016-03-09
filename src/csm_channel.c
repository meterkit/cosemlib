#include "csm_channel.h"
#include "csm_config.h"
#include "csm_services.h"


int csm_channel_init(csm_channel *channel)
{
    (void) channel;
    return FALSE;
}

int csm_channel_execute(csm_channel *channel, csm_asso_state *asso, csm_array *packet)
{
    int ret = FALSE;

    if (asso->state_cf == CF_ASSOCIATED)
    {
        ret = csm_services_execute(asso, &channel->request, packet);
    }
    else
    {
        ret = csm_asso_execute(asso, packet);
    }

    return ret;
}
