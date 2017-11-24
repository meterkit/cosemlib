/**
 * Cosem services coder/decoder
 *
 * Copyright (c) 2016, Anthony Rabine
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the BSD license.
 * See LICENSE.txt for more details.
 *
 */

#include "csm_services.h"
#include "csm_axdr_codec.h"

static csm_db_access_handler database = NULL;

// FIXME: add parameters to specialize the exception response
int svc_exception_response_encoder(csm_array *array)
{
    int valid = csm_array_write_u8(array, AXDR_EXCEPTION_RESPONSE);
    valid = valid && csm_array_write_u8(array, 1U);
    valid = valid && csm_array_write_u8(array, 1U);
    return valid;
}


enum data_access_result
{
    SRV_RESULT_SUCCESS              = 0U,
    SRV_RESULT_HARDWARE_FAULT       = 1U,
    SRV_RESULT_TEMPORARY_FAILURE    = 2U,
    SRV_RESULT_READ_WRITE_DENIED    = 3U,
    SRV_RESULT_OBJECT_UNDEFINED     = 4U,
    SRV_RESULT_OTHER_REASON         = 250U
};

int svc_data_access_result_encoder(csm_array *array, csm_db_code code)
{
    uint8_t result;
    // Transform the code into a DLMS/Cosem valid response
    if (code == CSM_OK)
    {
        result = SRV_RESULT_SUCCESS;
    }
    else
    {
        result = SRV_RESULT_OTHER_REASON;
    }

    return csm_array_write_u8(array, result);
}


int svc_decode_request(csm_request *request, csm_array *array)
{
    int valid = csm_array_read_u8(array, &request->type);
    valid = valid && csm_array_read_u8(array, &request->sender_invoke_id); // save the invoke ID to reuse the same
    valid = valid && csm_array_read_u16(array, &request->db_request.data.class_id);
    valid = valid && csm_array_read_buff(array, &request->db_request.data.obis.A, 6U);
    valid = valid && csm_array_read_u8(array, (uint8_t*)&request->db_request.data.id);
    valid = valid && csm_array_read_u8(array, &request->db_request.use_sel_access);

    return valid;
}

static csm_db_code svc_get_request_decoder(csm_asso_state *state, csm_request *request, csm_array *array)
{
    csm_db_code code = CSM_ERR_BAD_ENCODING;
    (void) state;

    CSM_LOG("[SVC] Decoding GET.request");

    if (svc_decode_request(request, array))
    {
        request->db_request.service = SVC_GET;
        if (database != NULL)
        {
            // Prepare the response
            array->wr_index = 0U;
            CSM_LOG("[SVC] Encoding GET.response");

            int valid = csm_array_write_u8(array, AXDR_GET_RESPONSE);
            valid = valid && csm_array_write_u8(array, SVC_GET_RESPONSE_NORMAL);
            valid = valid && csm_array_write_u8(array, request->sender_invoke_id);
            valid = valid && csm_array_write_u8(array, 0U); // data result

            // Actually append the data
            if (valid)
            {
                code = database(array, array, request);
                // FIXME: update the code according to the DB result
            }
        }
        else
        {
            CSM_ERR("[SVC] Database pointer not set");
            code = CSM_ERR_OBJECT_ERROR;
        }
    }

    if (code != CSM_OK)
    {
        array->wr_index = 0U;
        if (svc_exception_response_encoder(array))
        {
            code = CSM_OK;
        }
        else
        {
            CSM_ERR("[SVC] Internal problem, cannot encore exception response");
        }
    }

    return code;
}


static const uint32_t gResponseNormalHeaderSize = 6U; // Offset where data can be returned for an Action

static csm_db_code svc_set_or_action(csm_asso_state *state, csm_request *request, csm_array *array)
{
    csm_db_code code = CSM_ERR_BAD_ENCODING;
    (void) state;

    if (svc_decode_request(request, array))
    {
        if (database != NULL)
        {
            CSM_LOG("[SVC] Encoding SET/ACTION.response");

            // The output data will point to a different area into our working buffer
            // This will help us to encode the data
            csm_array output = *array;
            uint32_t reply_size = 0U;
            output.offset += gResponseNormalHeaderSize; // begin to encode the reply just after the response header
            output.rd_index = 0U;
            output.wr_index = 0U;

            code = database(array, &output, request);

            reply_size = output.wr_index;

            // Encode the response
            output.offset -= gResponseNormalHeaderSize;
            output.wr_index = 0U;

            uint8_t service_resp = (request->db_request.service == SVC_SET) ? AXDR_SET_RESPONSE : AXDR_ACTION_RESPONSE;
            int valid = csm_array_write_u8(&output, service_resp);
            valid = valid && csm_array_write_u8(&output, SRV_SET_RESPONSE_NORMAL); // FIXME: use proper service tag
            valid = valid && csm_array_write_u8(&output, request->sender_invoke_id);
            valid = svc_data_access_result_encoder(&output, code);

            if (request->db_request.service == SVC_ACTION)
            {
                // Encode additional data if any
                if (reply_size > 0U)
                {
                    valid = valid && csm_array_write_u8(&output, 1U); // presence flag for optional return-parameters
                    valid = valid && csm_array_write_u8(&output, 0U); // Data
                    valid = valid && csm_array_writer_jump(&output, reply_size); // Virtually add the data (already encoded in the buffer)
                }
                else
                {
                    valid = valid && csm_array_write_u8(&output, 0U); // presence flag for optional return-parameters
                }
            }

            // Update size to send to output channel
            array->wr_index = output.wr_index;

            if (!valid)
            {
                code = CSM_ERR_BAD_ENCODING;
            }
            else
            {
                code = CSM_OK;
            }
        }
        else
        {
            CSM_ERR("[SVC][SET] Database pointer not set");
            code = CSM_ERR_OBJECT_ERROR;
        }
    }

    if (code != CSM_OK)
    {
        array->wr_index = 0U;
        if (svc_exception_response_encoder(array))
        {
            code = CSM_OK;
        }
        else
        {
            CSM_ERR("[SVC][SET] Internal problem, cannot encore exception response");
        }
    }
    else
    {
        CSM_ERR("[SVC][SET] Encoding error");
    }

    return code;
}

static csm_db_code svc_set_request_decoder(csm_asso_state *state, csm_request *request, csm_array *array)
{
    request->db_request.service = SVC_SET;
    CSM_LOG("[SVC] Decoding SET.request");
    return svc_set_or_action(state, request, array);
}


static csm_db_code svc_action_request_decoder(csm_asso_state *state, csm_request *request, csm_array *array)
{
    request->db_request.service = SVC_ACTION;
    CSM_LOG("[SVC] Decoding ACTION.request");
    return svc_set_or_action(state, request, array);
}


typedef csm_db_code (*svc_decoder_func)(csm_asso_state *state, csm_request *request, csm_array *array);
typedef csm_db_code (*svc_encoder_func)(csm_asso_state *state, csm_request *request, csm_array *array);

typedef struct
{
    uint8_t tag;
    svc_decoder_func decoder;   //!< Used by the server implementation
    svc_encoder_func encoder;   //!< Used by the client implementation

} csm_service_handler;

static const csm_service_handler services[] =
{
    { AXDR_GET_REQUEST, svc_get_request_decoder, NULL },
    { AXDR_SET_REQUEST, svc_set_request_decoder, NULL },
    { AXDR_ACTION_REQUEST, svc_action_request_decoder, NULL },
};

#define NUMBER_OF_SERVICES (sizeof(services) / sizeof(services[0]))

void csm_services_init(const csm_db_access_handler db_access)
{
    database = db_access;
}

int csm_services_hls_execute(csm_asso_state *state, csm_request *request, csm_array *array)
{
    // FIXME: restrict only to the current association object and reply_to_hls_authentication method
    CSM_LOG("[SVC] Received HLS Pass 3 -- FIXME accept only current association object");

    return csm_server_services_execute(state, request, array);
}

int csm_server_services_execute(csm_asso_state *state, csm_request *request, csm_array *array)
{
    int number_of_bytes = 0;
    // FIXME: test the array size: minimum/maximum data size allowed
    if (database != NULL)
    {
        uint8_t tag;
        if (csm_array_read_u8(array, &tag))
        {
            for (uint32_t i = 0U; i < NUMBER_OF_SERVICES; i++)
            {
                const csm_service_handler *srv = &services[i];
                if ((srv->tag == tag) && (srv->decoder != NULL))
                {
                    CSM_LOG("[SVC] Found service");
                    if (srv->decoder(state, request, array) == CSM_OK)
                    {
                        number_of_bytes = array->wr_index;
                    }
                    else
                    {
                        CSM_ERR("[SVC] Encoding error!");
                    }
                    break;
                }
            }
        }
    }
    return number_of_bytes;
}


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

        if (valid)
        {
            response->access_result = CSM_ACCESS_RESULT_SUCCESS;
        }

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

Généré :

7EA04B000200250732723CE6E600


C001C100070100620102FF02

  01
    01
       0204
           0204
                12 0008
                09 06 00 00 01 00 00 FF
                0F 02
                12 0000
                      0007E10801FF000000008000
                      0007E10A17FF0E3702008000

                0100


  8CC97E


*/

int csm_client_encode_selective_access_by_range(csm_array *array, csm_object_t *restricting_object, csm_array *start, csm_array *end)
{
    int valid = csm_array_write_u8(array, 1U); // by range
    valid = valid && csm_array_write_u8(array, AXDR_TAG_STRUCTURE);
    valid = valid && csm_ber_write_len(array, 4U);

    // 1. restricting_object
    valid = valid && csm_axdr_wr_capture_object(array, restricting_object);

    // 2. start date
    valid = valid && csm_axdr_wr_octetstring(array, csm_array_rd_data(start), csm_array_written(start));
    // 3. end date
    valid = valid && csm_axdr_wr_octetstring(array, csm_array_rd_data(end), csm_array_written(end));

    // 4. selected values
    valid = valid && csm_array_write_u8(array, 0x01U); // selected values
    valid = valid && csm_array_write_u8(array, 0x00U); // array null

    return valid;
}




int svc_get_request_encoder(csm_request *request, csm_array *array)
{
    int valid = csm_array_write_u8(array, AXDR_GET_REQUEST);
    valid = valid && csm_array_write_u8(array, request->type);

    if (request->type == SVC_GET_REQUEST_NORMAL)
    {
        valid = valid && csm_array_write_u8(array, request->sender_invoke_id);
        valid = valid && csm_array_write_u16(array, request->db_request.data.class_id);
        valid = valid && csm_array_write_buff(array, (const uint8_t *)&request->db_request.data.obis.A, 6U);
        valid = valid && csm_array_write_u8(array, request->db_request.data.id);

        if (request->db_request.use_sel_access)
        {
            valid = valid && csm_array_write_u8(array, 1U); // use selective access
            if (request->db_request.access_params.buff != NULL)
            {
                valid = valid && csm_array_write_buff(array, request->db_request.access_params.buff, csm_array_written(&request->db_request.access_params));
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

typedef int (*srv_resp_decoder_func)(csm_response *response, csm_array *array);
typedef int (*srv_resp_encoder_func)(csm_response *response, csm_array *array);

typedef struct
{
    uint8_t tag;
    srv_resp_decoder_func decoder;   //!< Used by the server implementation
    srv_resp_encoder_func encoder;   //!< Used by the client implementation

} csm_client_service_handler;

static const csm_client_service_handler client_services[] =
{
    { AXDR_GET_RESPONSE, svc_get_response_decoder, NULL },
//    { AXDR_SET_REQUEST, srv_set_request_decoder, NULL },
//    { AXDR_ACTION_REQUEST, srv_action_request_decoder, NULL },
};

#define NUMBER_OF_CLIENT_SERVICES (sizeof(client_services) / sizeof(client_services[0]))

int csm_client_decode(csm_response *response, csm_array *array)
{
    int valid = FALSE;
    uint8_t tag;
    if (csm_array_read_u8(array, &tag))
    {
        for (uint32_t i = 0U; i < NUMBER_OF_CLIENT_SERVICES; i++)
        {
            const csm_client_service_handler *svc = &client_services[i];
            if ((svc->tag == tag) && (svc->decoder != NULL))
            {
                CSM_LOG("[SVC] Found service");
                valid = svc->decoder(response, array);
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


