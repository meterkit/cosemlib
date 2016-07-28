#include "db_cosem_clock.h"
#include "csm_axdr_codec.h"

static const uint8_t date_time[12] = {
    0x07, 0xD2,// year 2002
    0x0C,// month December
    0x04,// day 4th
    0x03,// day of the week, Wednesday
    0x0A, 0x06, 0x0B,// time 10:06:12
    0xFF,// hundredths not specified
    0x00, 0x78,// deviation 120 minutes
    0x00// status OK
};



csm_db_code db_cosem_clock_func(csm_array *in, csm_array *out, csm_request *request)
{
    csm_db_code code = CSM_ERR_OBJECT_ERROR;
    (void) in;

    if (request->db_request.service == SRV_GET)
    {
        if (csm_axdr_wr_octetstring(out, &date_time[0], 12U))
        {
            code = CSM_OK;
        }
    }
    else if (request->type == SRV_SET)
    {
        // Not implemented
    }

    return code;
}
