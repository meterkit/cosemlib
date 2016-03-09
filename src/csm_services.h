#ifndef CSM_SERVICES_H
#define CSM_SERVICES_H

#include "csm_definitions.h"
#include "csm_association.h"


enum csm_service { GET, SET, ACTION };


typedef struct
{
    enum csm_service service;
    csm_selective_access access;
    csm_object object;
    uint8_t sender_invoke_id;
    csm_llc llc;

} csm_request;

void csm_services_init(const csm_db_access *db_access);

// Return he number of bytes to transfer back, 0 if no response
int csm_services_execute(csm_asso_state *state, csm_request *request, csm_array *array);

#endif // CSM_SERVICES_H
