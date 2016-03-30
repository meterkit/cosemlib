#ifndef CSM_SERVICES_H
#define CSM_SERVICES_H

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
    CSM_ERR_OBJECT_ERROR,   //!< Generic error coming from the object

    // Some more specific errors
    CSM_ERR_OBJECT_NOT_FOUND,    //!< Not found in the database, check obis code!
    CSM_ERR_BAD_ENCODING,        //!< Bad encoding of codec
    CSM_ERR_UNAUTHORIZED_ACCESS, //!< Attribute access problem
    CSM_ERR_TEMPORARY_FAILURE,   ///< Temporary failure
    CSM_ERR_DATA_CONTENT_NOT_OK, ///< Data content is not accepted.
    CSM_ERR_APDU_BUFFER_FULL,    ///< Apdu is full.
    CSM_ERR_FRAGMENTATION_USED   ///< Fragmentation used, request not completly performed
} csm_db_code;

typedef csm_db_code (*csm_db_access_handler)(csm_array *array, csm_request *request);

void csm_services_init(const csm_db_access_handler db_access);

// Return he number of bytes to transfer back, 0 if no response
int csm_services_execute(csm_asso_state *state, csm_request *request, csm_array *array);

// Specific method in case of HLS authentication
int csm_services_hls_execute(csm_asso_state *state, csm_request *request, csm_array *array);

#endif // CSM_SERVICES_H
