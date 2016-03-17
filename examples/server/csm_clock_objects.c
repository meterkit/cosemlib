#include "csm_clock_objects.h"
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



csm_db_code csm_db_clock_func(csm_array *array, csm_request *request)
{
    // FIXME: filter thanks to the request parameters (service, ...)
    (void) request;
    csm_db_code code = CSM_ERR_OBJECT_ERROR;
    if (axdr_encode_octet_string(array, &date_time[0], 12U))
    {
        code = CSM_OK;
    }
    return code;
}
