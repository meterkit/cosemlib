#include "csm_services.h"
#include "csm_axdr_codec.h"

static const csm_db_access *database = NULL;

/*
Get-Request ::= CHOICE
{
    get-request-normal                 [1] IMPLICIT    Get-Request-Normal,
    get-request-next                   [2] IMPLICIT    Get-Request-Next,
    get-request-with-list              [3] IMPLICIT    Get-Request-With-List
}

Get-Request-Normal ::= SEQUENCE
{
    invoke-id-and-priority             Invoke-Id-And-Priority,
    cosem-attribute-descriptor         Cosem-Attribute-Descriptor,
    access-selection                   Selective-Access-Descriptor OPTIONAL
}

-- IEC 61334-6:2000 Clause 5 specifies that bits of any byte are numbered from 1 to 8,
-- where bit 8 is the most significant.
-- In the DLMS UA Green Book, bits are numbered from 0 to 7.
-- Use of Invoke-Id-And-Priority
--    invoke-id                bits 0-3
--    reserved                 bits 4-5
--    service-class            bit  6        0 = Unconfirmed, 1 = Confirmed
--    priority                 bit  7        0 = Normal, 1 = High
Invoke-Id-And-Priority ::=             Unsigned8


Cosem-Attribute-Descriptor ::= SEQUENCE
{
    class-id                           Cosem-Class-Id,
    instance-id                        Cosem-Object-Instance-Id,
    attribute-id                       Cosem-Object-Attribute-Id
}

Cosem-Class-Id ::=                     Unsigned16
Cosem-Object-Instance-Id ::=           OCTET STRING (SIZE(6))
Cosem-Object-Attribute-Id ::=          Integer8
Cosem-Object-Method-Id ::=             Integer8

*/



static csm_db_code get_service_decoder(csm_asso_state *state, csm_request *request, csm_array *array)
{
    csm_db_code code = CSM_ERR_BAD_ENCODING;
    (void) state;

    CSM_LOG("[SRV] Decoding GET.request");

    int valid = csm_array_read_u8(array, &request->sender_invoke_id); // save the invoke ID to reuse the same
    valid = valid && csm_array_read_u16(array, &request->object.class_id);
    valid = valid && csm_array_read_buff(array, &request->object.obis.A, 6U);
    valid = valid && csm_array_read_u8(array, (uint8_t*)&request->object.attribute.attribute_id);

    if (valid)
    {
        code = CSM_OK;
    }

    return code;
}

/*
 *
 *
 *

Get-Response-Normal ::= SEQUENCE
{
invoke-id-and-priority             Invoke-Id-And-Priority,
result                             Get-Data-Result
}

Get-Data-Result ::= CHOICE
{
data                                [0] Data,
data-access-result                  [1] IMPLICIT Data-Access-Result
}

C401//GET.response.normal
81// invoke-id and priority
00// Get-Data-Result choice data
0906// octet string (6)
0000010000FF// logical name, OBIS code of the clock

 */

/*
 *
Get-Response ::= CHOICE
{
    get-response-normal                [1] IMPLICIT    Get-Response-Normal,
    get-response-with-datablock        [2] IMPLICIT    Get-Response-With-Datablock,
    get-response-with-list             [3] IMPLICIT    Get-Response-With-List
}
*/
enum srv_get_response
{
    SRV_GET_RESPONSE_NORMAL         = 1U,
    SRV_GET_RESPONSE_WITH_DATABLOCK = 2U,
    SRV_GET_RESPONSE_WITH_LIST      = 3U
};

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

static csm_db_code get_service_encoder(csm_asso_state *state, csm_request *request, csm_array *array)
{
    csm_db_code code = CSM_ERR_BAD_ENCODING;
    (void) state;

    CSM_LOG("[SRV] Encoding GET.response");

    int valid = csm_array_write_u8(array, AXDR_GET_RESPONSE);
    valid = valid && csm_array_write_u8(array, SRV_GET_RESPONSE_NORMAL);
    valid = valid && csm_array_write_u8(array, request->sender_invoke_id);
    valid = valid && csm_array_write_u8(array, 0U); // data result
    valid = valid && axdr_encode_octet_string(array, &date_time[0], 12U);

    if (valid)
    {
        code = CSM_OK;
    }
    return code;
}


typedef csm_db_code (*srv_decoder_func)(csm_asso_state *state, csm_request *request, csm_array *array);
typedef csm_db_code (*srv_encoder_func)(csm_asso_state *state, csm_request *request, csm_array *array);

typedef struct
{
    uint8_t tag;
    srv_decoder_func decoder;
    srv_encoder_func encoder;

} csm_service_handler;

static const csm_service_handler services[] =
{
    { AXDR_GET_REQUEST, get_service_decoder, get_service_encoder }
};

#define NUMBER_OF_SERVICES (sizeof(services) / sizeof(csm_service_handler))

void csm_services_init(const csm_db_access *db_access)
{
    database = db_access;
}


int csm_services_execute(csm_asso_state *state, csm_request *request, csm_array *array)
{
    int number_of_bytes = 0;
    // FIXME: add a minimum data size
    if (database != NULL)
    {
        for (uint32_t i = 0U; i < NUMBER_OF_SERVICES; i++)
        {
            const csm_service_handler *srv = &services[i];

            uint8_t tag;
            if (csm_array_read_u8(array, &tag))
            {
                if ((srv->tag == tag) && (srv->decoder != NULL))
                {
                    CSM_LOG("[SRV] Found service");
                    if (srv->decoder(state, request, array) == CSM_OK)
                    {
                        if (srv->encoder != NULL)
                        {
                            array->wr_index = 0U;
                            if (srv->encoder(state, request, array) == CSM_OK)
                            {
                                number_of_bytes = array->wr_index;
                            }
                            else
                            {
                                CSM_ERR("[SRV] Encoding error!");
                            }
                        }
                    }
                }
            }
        }
    }
    return number_of_bytes;
}
