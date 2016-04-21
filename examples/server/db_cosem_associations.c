#include "db_cosem_associations.h"
#include "csm_channel.h"
#include "csm_axdr_codec.h"

csm_db_code db_cosem_associations_func(csm_array *array, csm_request *request)
{
    csm_db_code code = CSM_ERR_OBJECT_ERROR;

    (void) array;

    if (request->db_request.service == SRV_GET)
    {

    }
    else if (request->db_request.service == SRV_SET)
    {
        // Not implemented
    }
    else
    {
        // Action
        if (csm_axdr_rd_octetstring(array))
        {
            CSM_LOG("[DB] Reply to HLS authentication");
            csm_channel_hls_pass3(array, request);
        }
    }

    return code;
}

