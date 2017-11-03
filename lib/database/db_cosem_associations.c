#include "db_cosem_associations.h"
#include "csm_channel.h"
#include "csm_axdr_codec.h"

csm_db_code db_cosem_associations_func(csm_array *in, csm_array *out, csm_request *request)
{
    csm_db_code code = CSM_ERR_OBJECT_ERROR;

    if (request->db_request.service == SVC_GET)
    {

    }
    else if (request->db_request.service == SVC_SET)
    {
        // Not implemented
    }
    else
    {
        // Action
        if (csm_axdr_rd_octetstring(in))
        {
            CSM_LOG("[DB] Reply to HLS authentication");
            int ret = csm_channel_hls_pass3(in, request);
            ret = ret && csm_channel_hls_pass4(out, request);

            if (ret)
            {
                code = CSM_OK;
            }
        }
    }

    return code;
}

