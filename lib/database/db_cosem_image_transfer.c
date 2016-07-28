#include "db_cosem_image_transfer.h"
#include "csm_axdr_codec.h"


typedef struct
{
    uint8_t enable;

} app_fup_config;


csm_db_code db_cosem_image_transfer_func(csm_array *in, csm_array *out, csm_request *request)
{
    csm_db_code code = CSM_ERR_OBJECT_ERROR;
    (void) in;

    if (request->db_request.service == SRV_GET)
    {
        switch(request->db_request.data.id)
        {
            case 5:
            if (csm_axdr_wr_boolean(out, 0U))
            {
                code = CSM_OK;
            }
            break;

            default:
                break;
        }


    }
    else if (request->type == SRV_SET)
    {
        // Not implemented
    }

    return code;
}
