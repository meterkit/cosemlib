

int srv_decode_response(csm_response *response, csm_array *array)
{
    int valid = csm_array_read_u8(array, &response->type);
//    valid = valid && csm_array_read_u8(array, &response->invoke_id);
//    valid = valid && csm_array_read_u16(array, &response->result);

    return valid;
}

static csm_db_code srv_get_response_decoder(csm_asso_state *state, csm_request *request, csm_array *array)
{
    csm_db_code code = CSM_ERR_BAD_ENCODING;
    (void) state;
    csm_response response;

    CSM_LOG("[SRV] Decoding GET.response");

    if (srv_decode_response(&response, array))
    {
        if (response.result == 0)
        {
            // Decode data

        }
        else
        {


            code = CSM_ERR_OBJECT_ERROR;
        }
    }


    return code;
}
