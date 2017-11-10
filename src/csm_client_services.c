
#include "csm_services.h"
#include "csm_axdr_codec.h"

int svc_is_standard_result(uint8_t result)
{
    int valid = FALSE;
    switch (result)
    {
        case CSM_ACCESS_RESULT_SUCCESS:
        case CSM_ACCESS_RESULT_HARDWARE_FAULT:
        case CSM_ACCESS_RESULT_TEMPORARY_FAILURE:
        case CSM_ACCESS_RESULT_READ_WRITE_DENIED:
        case CSM_ACCESS_RESULT_OBJECT_UNDEFINED:
        case CSM_ACCESS_RESULT_OBJECT_CLASS_INCONSISTENT:
        case CSM_ACCESS_RESULT_OBJECT_UNAVAILABLE:
        case CSM_ACCESS_RESULT_TYPE_UNMATCHED:
        case CSM_ACCESS_RESULT_SCOPE_OF_ACCESS_VIOLATED:
        case CSM_ACCESS_RESULT_DATA_BLOCK_UNAVAILABLE:
        case CSM_ACCESS_RESULT_LONG_GET_ABORTED:
        case CSM_ACCESS_RESULT_NO_LONG_GET_IN_PROGRESS:
        case CSM_ACCESS_RESULT_LONG_SET_ABORTED:
        case CSM_ACCESS_RESULT_NO_LONG_SET_IN_PROGRESS:
        case CSM_ACCESS_RESULT_DATA_BLOCK_NUMBER_INVALID:
        case CSM_ACCESS_RESULT_OTHER_REASON:
            valid = TRUE;
            break;
        default:
            break;
    }
    return valid;
}


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

        if (valid)
        {
            if (response->result == 0)
            {
                response->access_result = CSM_ACCESS_RESULT_SUCCESS;
            }
            else if (response->result == 1)
            {
                // details about the error is on the next byte
                uint8_t result;
                valid = csm_array_read_u8(array, &result);
                response->access_result = (csm_data_access_result)result;
                valid = valid && svc_is_standard_result(response->access_result);
            }
            else
            {
                valid = FALSE;
            }
        }
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


/*
Get request with selective access by range (date)

 7EA04F000200230332BDE2E6E600
C0 01 C1 0007 00 01 62 02 00 FF 02
  01 Use sel access
    01 by range
       02 04 structure of 4

            02 04
                    12 0008 class id
                    09 06 00 00 01 00 00 FF
                    0F 02 attribute index
                    12 0000  data_index

                09 0C 07E1080107000000FFFFC400
                09 0C 07E10B0107000000FFFFC400

                01 00 selected_values (array null)

4CD77E
*/


int svc_get_request_encoder(csm_request *request, csm_array *array)
{
    int valid = csm_array_write_u8(array, AXDR_GET_REQUEST);
    valid = valid && csm_array_write_u8(array, request->type);

    if (request->type == SVC_GET_REQUEST_NORMAL)
    {
        valid = valid && csm_array_write_u8(array, request->sender_invoke_id); // save the invoke ID to reuse the same
        valid = valid && csm_array_write_u16(array, request->db_request.data.class_id);
        valid = valid && csm_array_write_buff(array, (const uint8_t *)&request->db_request.data.obis.A, 6U);
        valid = valid && csm_array_write_u8(array, request->db_request.data.id);

        if (request->db_request.access.use_sel_access)
        {
            valid = valid && csm_array_write_u8(array, 1U);

            if (request->db_request.access.access_params.buff != NULL)
            {
                valid = valid && csm_array_write_buff(array, request->db_request.access.access_params.buff, csm_array_written(&request->db_request.access.access_params));
            }
            else
            {
                valid = FALSE;
            }
        }
        else
        {
            valid = valid && csm_array_write_u8(array, 0U); // no selective access
        }
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

int csm_client_decode(csm_response *response, csm_array *array)
{
    int valid = FALSE;
    uint8_t tag;
    if (csm_array_read_u8(array, &tag))
    {
        for (uint32_t i = 0U; i < NUMBER_OF_SERVICES; i++)
        {
            const csm_service_handler *srv = &services[i];
            if ((srv->tag == tag) && (srv->decoder != NULL))
            {
                CSM_LOG("[SVC] Found service");
                valid = srv->decoder(response, array);
                break;
            }
        }
    }

    return valid;
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


int csm_client_encode_selective_access_by_range(csm_array *array, csm_object_t *restricting_object)
{
    int valid = csm_array_write_u8(array, AXDR_TAG_STRUCTURE);
    valid = valid && csm_ber_write_len(array, 4U);
    valid = valid && csm_axdr_wr_capture_object(array, restricting_object);

    return valid;
}


