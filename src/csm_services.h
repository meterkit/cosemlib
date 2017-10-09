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
    CSM_OK_BLOCK,           //!< Request OK, ask for a block transfer (not enough space to store the data)
    CSM_ERR_OBJECT_ERROR,   //!< Generic error coming from the object

    // Some more specific errors
    CSM_ERR_OBJECT_NOT_FOUND,    //!< Not found in the database, check obis code!
    CSM_ERR_BAD_ENCODING,        //!< Bad encoding of codec
    CSM_ERR_UNAUTHORIZED_ACCESS, //!< Attribute access problem
    CSM_ERR_TEMPORARY_FAILURE,   ///< Temporary failure
    CSM_ERR_DATA_CONTENT_NOT_OK, ///< Data content is not accepted.
} csm_db_code;


/*
    Data-Access-Result ::= ENUMERATED
    {
    success                            (0),
    hardware-fault                     (1),
    temporary-failure                  (2),
    read-write-denied                  (3),
    object-undefined                   (4),
    object-class-inconsistent          (9),
    object-unavailable                 (11),
    type-unmatched                     (12),
    scope-of-access-violated            (13),
    data-block-unavailable             (14),
    long-get-aborted                   (15),
    no-long-get-in-progress            (16),
    long-set-aborted                   (17),
    no-long-set-in-progress            (18),
    data-block-number-invalid          (19),
    other-reason                       (250)
    }
    */
typedef enum
{
    CSM_ACCESS_RESULT_SUCCESS = 0,
    CSM_ACCESS_RESULT_HARDWARE_FAULT = 1,
    CSM_ACCESS_RESULT_TEMPORARY_FAILURE = 2,
    CSM_ACCESS_RESULT_READ_WRITE_DENIED = 3,
} csm_data_access_result;

typedef csm_db_code (*csm_db_access_handler)(csm_array *in, csm_array *out, csm_request *request);

void csm_services_init(const csm_db_access_handler db_access);

// Return he number of bytes to transfer back, 0 if no response
int csm_services_execute(csm_asso_state *state, csm_request *request, csm_array *array);

// Specific method in case of HLS authentication
int csm_services_hls_execute(csm_asso_state *state, csm_request *request, csm_array *array);

#endif // CSM_SERVICES_H
