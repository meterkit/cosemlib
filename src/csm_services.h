#ifndef CSM_SERVICES_H
#define CSM_SERVICES_H

#ifdef __cplusplus
extern "C" {
#endif

#include "csm_definitions.h"
#include "csm_association.h"

enum csm_req_type { RQ_NORMAL, RQ_NEXT, RQ_WITH_LIST };

/**
 * @brief Generic Codec error codes
 **/
typedef enum
{
    // These two error codes can be used in priority
    CSM_OK,                 //!< Request OK
    CSM_OK_BLOCK,           //!< Request OK, ask for a block transfer (not enough space to store the data)
    CSM_ERR_OBJECT_ERROR,   //!< Generic error coming from the object

    // Some more specific errors
    CSM_ERR_OBJECT_NOT_FOUND,    //!< Not found in the database, check obis code!
    CSM_ERR_BAD_ENCODING,        //!< Bad encoding of codec
    CSM_ERR_UNAUTHORIZED_ACCESS, //!< Attribute access problem
    CSM_ERR_TEMPORARY_FAILURE,   ///< Temporary failure
    CSM_ERR_DATA_CONTENT_NOT_OK, ///< Data content is not accepted.
} csm_db_code;


enum svc_response
{
    SVC_GET_RESPONSE_NORMAL         = 1U,
    SVC_GET_RESPONSE_WITH_DATABLOCK = 2U,
    SVC_GET_RESPONSE_WITH_LIST      = 3U,
    SRV_SET_RESPONSE_NORMAL         = 1U,
    SRV_ACTION_RESPONSE_NORMAL      = 1U
};

enum svc_request
{
    SVC_GET_REQUEST_NORMAL         = 1U,
    SVC_GET_REQUEST_NEXT           = 2U,
    SVC_GET_REQUEST_WITH_LIST      = 3U
};



typedef csm_db_code (*csm_db_access_handler)(csm_array *in, csm_array *out, csm_request *request);




// ----------------------------------- CLIENT SERVICES -----------------------------------

void csm_client_init(csm_request *request, csm_response *response);
int csm_client_has_more_data(csm_response *response);
int csm_client_decode(csm_response *response, csm_array *array);
int csm_client_encode(csm_request *request, csm_array *array);
int svc_get_request_encoder(csm_request *request, csm_array *array);

// ----------------------------------- SERVER SERVICES -----------------------------------

void csm_services_init(const csm_db_access_handler db_access);

// Return he number of bytes to transfer back, 0 if no response
int csm_server_services_execute(csm_asso_state *state, csm_request *request, csm_array *array);

// Specific method in case of HLS authentication
int csm_services_hls_execute(csm_asso_state *state, csm_request *request, csm_array *array);

#ifdef __cplusplus
}
#endif

#endif // CSM_SERVICES_H
