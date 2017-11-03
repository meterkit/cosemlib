
#include "csm_services.h"
#include "csm_axdr_codec.h"

//
// Get-Response-WithDataBlock: C4 02 C1 0000000001008201F40601001B0800FF15000000000
static int svc_get_response_decoder(csm_response *response, csm_array *array)
{
    CSM_LOG("[SVC] Decoding GET.response");

    int valid = csm_array_read_u8(array, &response->type);

    if (response->type == SVC_GET_RESPONSE_NORMAL)
    {
        // Get-Response-Normal
        valid = valid && csm_array_read_u8(array, &response->invoke_id);
        valid = valid && csm_array_read_u8(array, &response->result);
    }
    else if (response->type == SVC_GET_RESPONSE_WITH_DATABLOCK)
    {
        // Get-Response-WithDataBlock
        valid = valid && csm_array_read_u8(array, &response->invoke_id);
        valid = valid && csm_array_read_u8(array, &response->last_block);
        valid = valid && csm_array_read_u32(array, &response->block_number);
        // Now the read pointer is on the data
    }
    else
    {
        CSM_LOG("[SVC] Service not supported");
    }

    return valid;
}


static int svc_get_request_encoder(csm_request *request, csm_array *array)
{
    int valid = csm_array_write_u8(array, AXDR_GET_REQUEST);
    valid = valid && csm_array_write_u8(array, request->type);

    if (request->type == SVC_GET_REQUEST_NORMAL)
    {
        valid = valid && csm_array_write_u8(array, request->sender_invoke_id); // save the invoke ID to reuse the same
        valid = valid && csm_array_write_u16(array, request->db_request.data.class_id);
        valid = valid && csm_array_write_buff(array, (const uint8_t *)&request->db_request.data.obis.A, 6U);
        valid = valid && csm_array_write_u8(array, request->db_request.data.id);
        valid = valid && csm_array_write_u8(array, request->db_request.access.use_sel_access);
    }
    else if (request->type == SVC_GET_REQUEST_NEXT)
    {
        valid = valid && csm_array_write_u8(array, request->sender_invoke_id); // save the invoke ID to reuse the same
        valid = valid && csm_array_write_u32(array, request->db_request.block_number); // save the invoke ID to reuse the same
    }
    else
    {
        CSM_LOG("[SVC] Service not supported");
    }
    return valid;
}

typedef int (*srv_decoder_func)(csm_response *response, csm_array *array);
typedef int (*srv_encoder_func)(csm_response *response, csm_array *array);

typedef struct
{
    uint8_t tag;
    srv_decoder_func decoder;   //!< Used by the server implementation
    srv_encoder_func encoder;   //!< Used by the client implementation

} csm_service_handler;

static const csm_service_handler services[] =
{
    { AXDR_GET_RESPONSE, svc_get_response_decoder, NULL },
//    { AXDR_SET_REQUEST, srv_set_request_decoder, NULL },
//    { AXDR_ACTION_REQUEST, srv_action_request_decoder, NULL },
};

#define NUMBER_OF_SERVICES (sizeof(services) / sizeof(services[0]))

csm_data_access_result csm_client_decode(csm_response *response, csm_array *array)
{
    csm_data_access_result status = CSM_ACCESS_RESULT_HARDWARE_FAULT;

    uint8_t tag;
    if (csm_array_read_u8(array, &tag))
    {
        for (uint32_t i = 0U; i < NUMBER_OF_SERVICES; i++)
        {
            const csm_service_handler *srv = &services[i];
            if ((srv->tag == tag) && (srv->decoder != NULL))
            {
                CSM_LOG("[SVC] Found service");
                if (srv->decoder(response, array))
                {
                    status = CSM_ACCESS_RESULT_SUCCESS;
                }
                else
                {
                    CSM_ERR("[SVC] Encoding error!");
                }
                break;
            }
        }
    }

    return status;
}

void csm_client_init(csm_request *request, csm_response *response)
{
    (void) request;

    response->type = 0U;
    response->block_number = 0U;
    response->invoke_id = 0U;
    response->result = 0U;
    response->last_block = 0U;
}

int csm_client_has_more_data(csm_response *response)
{
    int more_data = 0;
    if ((response->type == 2) &&
        (!response->last_block))
    {
        more_data = 1;
    }
    return more_data;
}

int csm_client_encode(csm_request *request, csm_array *array)
{
    return svc_get_request_encoder(request, array);
}


