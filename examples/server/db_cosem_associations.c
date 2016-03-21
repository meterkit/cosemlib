#include "db_cosem_associations.h"


csm_db_code db_cosem_associations_func(csm_array *array, csm_request *request)
{
    csm_db_code code = CSM_ERR_OBJECT_ERROR;

    if (request->db_request.service == SRV_GET)
    {

    }
    else if (request->type == SRV_SET)
    {
        // Not implemented
    }

    return code;
}

