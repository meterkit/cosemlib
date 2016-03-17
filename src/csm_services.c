#include "csm_services.h"
#include "csm_axdr_codec.h"

static csm_db_access_handler database = NULL;

/*
ExceptionResponse ::= SEQUENCE
{
    state-error                        [0] IMPLICIT ENUMERATED
    {
        service-not-allowed                 (1),
        service-unknown                     (2)
    },
    service-error                      [1] IMPLICIT ENUMERATED
    {
        operation-not-possible              (1),
        service-not-supported               (2),
        other-reason                        (3)
    }
}
*/

// FIXME: add parameters
int srv_exception_response_encoder(csm_array *array)
{
    int valid = csm_array_write_u8(array, AXDR_EXCEPTION_RESPONSE);
    valid = valid && csm_array_write_u8(array, 1U);
    valid = valid && csm_array_write_u8(array, 1U);
    return valid;
}


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

Selective-Access-Descriptor ::= SEQUENCE
{
    access-selector                    Unsigned8,
    access-parameters  Data
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


static csm_db_code srv_get_request_decoder(csm_asso_state *state, csm_request *request, csm_array *array)
{
    csm_db_code code = CSM_ERR_BAD_ENCODING;
    (void) state;

    CSM_LOG("[SRV] Decoding GET.request");

    int valid = csm_array_read_u8(array, &request->type);
    valid = valid && csm_array_read_u8(array, &request->sender_invoke_id); // save the invoke ID to reuse the same
    valid = valid && csm_array_read_u16(array, &request->db_request.data.class_id);
    valid = valid && csm_array_read_buff(array, &request->db_request.data.obis.A, 6U);
    valid = valid && csm_array_read_u8(array, (uint8_t*)&request->db_request.data.id);
    valid = valid && csm_array_read_u8(array, &request->db_request.access.use_sel_access);

    if (valid)
    {
        if (database != NULL)
        {
            // Prepare the response
            array->wr_index = 0U;
            CSM_LOG("[SRV] Encoding GET.response");

            int valid = csm_array_write_u8(array, AXDR_GET_RESPONSE);
            valid = valid && csm_array_write_u8(array, SRV_GET_RESPONSE_NORMAL);
            valid = valid && csm_array_write_u8(array, request->sender_invoke_id);
            valid = valid && csm_array_write_u8(array, 0U); // data result

            // Actually append the data
            if (valid)
            {
                code = database(array, request);
            }
        }
        else
        {
            CSM_ERR("[SRV] Database pointer not set");
            code = CSM_ERR_OBJECT_ERROR;
        }
    }

    if (code != CSM_OK)
    {
        array->wr_index = 0U;
        if (srv_exception_response_encoder(array))
        {
            code = CSM_OK;
        }
        else
        {
            CSM_ERR("[SRV] Internal problem, cannot encore exception response");
        }
    }

    return code;
}


/*

Set-Request ::= CHOICE
{
    set-request-normal                         [1] IMPLICIT  Set-Request-Normal,
    set-request-with-first-datablock           [2] IMPLICIT  Set-Request-With-First-Datablock,
    set-request-with-datablock                 [3] IMPLICIT  Set-Request-With-Datablock,
    set-request-with-list                      [4] IMPLICIT  Set-Request-With-List,
    set-request-with-list-and-first-datablock  [5] IMPLICIT  Set-Request-With-List-And-First-Datablock
}
Set-Request-Normal ::= SEQUENCE
{
    invoke-id-and-priority             Invoke-Id-And-Priority,
    cosem-attribute-descriptor         Cosem-Attribute-Descriptor,
    access-selection                   Selective-Access-Descriptor OPTIONAL,
    value                              Data
}
Set-Request-With-First-Datablock ::= SEQUENCE
{
    invoke-id-and-priority             Invoke-Id-And-Priority,
    cosem-attribute-descriptor         Cosem-Attribute-Descriptor,
    access-selection                   [0] IMPLICIT Selective-Access-Descriptor OPTIONAL,
    datablock                          DataBlock-SA
}
Set-Request-With-Datablock ::= SEQUENCE
{
    invoke-id-and-priority             Invoke-Id-And-Priority,
    datablock                          DataBlock-SA
}
Set-Request-With-List ::= SEQUENCE
{
    invoke-id-and-priority             Invoke-Id-And-Priority,
    attribute-descriptor-list          SEQUENCE OF Cosem-Attribute-Descriptor-With-Selection,
    value-list                         SEQUENCE OF Data
}
Set-Request-With-List-And-First-Datablock ::= SEQUENCE
{
    invoke-id-and-priority             Invoke-Id-And-Priority,
    attribute-descriptor-list          SEQUENCE OF Cosem-Attribute-Descriptor-With-Selection,
    datablock                          DataBlock-SA
}
Set-Response ::= CHOICE
{
    set-response-normal                     [1] IMPLICIT   Set-Response-Normal,
    set-response-datablock                  [2] IMPLICIT   Set-Response-Datablock,
    set-response-last-datablock             [3] IMPLICIT   Set-Response-Last-Datablock,
    set-response-last-datablock-with-list   [4] IMPLICIT   Set-Response-Last-Datablock-With-List,
    set-response-with-list                  [5] IMPLICIT   Set-Response-With-List
}
Set-Response-Normal ::= SEQUENCE
{
    invoke-id-and-priority             Invoke-Id-And-Priority,
    result                             Data-Access-Result
}
Set-Response-Datablock ::= SEQUENCE
{
    invoke-id-and-priority             Invoke-Id-And-Priority,
    block-number                       Unsigned32
}
Set-Response-Last-Datablock ::= SEQUENCE
{
    invoke-id-and-priority             Invoke-Id-And-Priority,
    result                             Data-Access-Result,
    block-number                       Unsigned32
}
Set-Response-Last-Datablock-With-List ::= SEQUENCE
{
    invoke-id-and-priority             Invoke-Id-And-Priority,
    result                             SEQUENCE OF Data-Access-Result,
    block-number                       Unsigned32
}
Set-Response-With-List ::= SEQUENCE
{
    invoke-id-and-priority   Invoke-Id-And-Priority,
    result                             SEQUENCE OF Data-Access-Result
}

 */




typedef csm_db_code (*srv_decoder_func)(csm_asso_state *state, csm_request *request, csm_array *array);
typedef csm_db_code (*srv_encoder_func)(csm_asso_state *state, csm_request *request, csm_array *array);

typedef struct
{
    uint8_t tag;
    srv_decoder_func decoder;   //!< Used by the server implementation
    srv_encoder_func encoder;   //!< Used by the client implementation

} csm_service_handler;

static const csm_service_handler services[] =
{
    { AXDR_GET_REQUEST, srv_get_request_decoder, NULL },

};

#define NUMBER_OF_SERVICES (sizeof(services) / sizeof(csm_service_handler))

void csm_services_init(const csm_db_access_handler db_access)
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
                        number_of_bytes = array->wr_index;
                    }
                    else
                    {
                        CSM_ERR("[SRV] Encoding error!");
                    }
                    break;
                }
            }
        }
    }
    return number_of_bytes;
}
