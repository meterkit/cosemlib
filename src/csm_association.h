#ifndef CSM_ASSOCIATION_H
#define CSM_ASSOCIATION_H

#include <stdint.h>
#include "csm_config.h"
#include "csm_ber.h"
#include "csm_definitions.h"

// States machine of the Control Function
enum state_cf { CF_INACTIVE, CF_IDLE, CF_ASSOCIATION_PENDING, CF_ASSOCIATED, CF_ASSOCIATION_RELEASE_PENDING };
// Type of services
enum type_service { REQUEST, INDICATION, RESPONSE, CONFIRM };
// Type of COSEM-GET service
enum type_get { GET_NORMAL, GET_WITH_LIST, GET_ONE_BLOCK, GET_LAST_BLOCK };

/*
-- xDLMS APDU-s used during Association establishment
InitiateRequest ::= SEQUENCE
{
--  shall not be encoded in DLMS without ciphering
dedicated-key                      OCTET STRING OPTIONAL,
response-allowed                   BOOLEAN DEFAULT TRUE,
proposed-quality-of-service        [0] IMPLICIT Integer8 OPTIONAL,
proposed-dlms-version-number       Unsigned8,
proposed-conformance               Conformance, -- Shall be encoded in BER
client-max-receive-pdu-size        Unsigned16
}

-- The Conformance field shall be encoded in BER. See IEC 61334-6 Example 1.

*/

typedef struct
{
    uint8_t dedicated_key[SIZE_OF_DEDICATED_KEY];
    uint8_t response_allowed;
    int8_t proposed_quality_of_service;
    uint8_t proposed_dlms_version_number;
    uint32_t proposed_conformance;
    uint16_t client_max_receive_pdu_size;
} csm_initiate_request;

/*
AARQ ::= [APPLICATION 0] IMPLICIT SEQUENCE
{
-- [APPLICATION 0] == [ 60H ] = [ 96 ]
protocol-version                   [0] IMPLICIT        BIT STRING {version1 (0)} DEFAULT {version1},
application-context-name           [1]                 Application-context-name,
called-AP-title                    [2]                 AP-title OPTIONAL,
called-AE-qualifier                [3]                 AE-qualifier OPTIONAL,
called-AP-invocation-id            [4]                 AP-invocation-identifier OPTIONAL,
called-AE-invocation-id            [5]                 AE-invocation-identifier OPTIONAL,
calling-AP-title                   [6]                 AP-title OPTIONAL,
calling-AE-qualifier               [7]                 AE-qualifier OPTIONAL,
calling-AP-invocation-id           [8]                 AP-invocation-identifier OPTIONAL,
calling-AE-invocation-id           [9]                 AE-invocation-identifier OPTIONAL,
-- The following field shall not be present if only the kernel is used.
sender-acse-requirements           [10] IMPLICIT      ACSE-requirements OPTIONAL,
-- The following field shall only be present if the authentication functional unit is selected.
mechanism-name                     [11] IMPLICIT      Mechanism-name OPTIONAL,
-- The following field shall only be present if the authentication functional unit is selected.
calling-authentication-value       [12] EXPLICIT      Authentication-value OPTIONAL,
implementation-information         [29] IMPLICIT      Implementation-data OPTIONAL,
user-information                   [30] EXPLICIT      Association-information OPTIONAL
}

-- The user-information field shall carry an InitiateRequest APDU encoded in A-XDR, and then
-- encoding the resulting OCTET STRING in BER.

*/

typedef struct
{
    uint16_t ssap; //< Client
    uint16_t dsap; //< Server
} csm_llc;

/**
 * @brief Configuration structure of one association
 */
typedef struct
{
    csm_llc llc;
    uint8_t channels;               ///< Bit mask of allowed channels ; up to 8 physical channels
    uint32_t conformance;          ///< All services and functionalities authorized.
    uint8_t  is_auto_connected;    ///< Boolean to indicate if the association is auto connected or not;
    uint8_t  authentication_mask;  ///< The mask of authentication authorized.
    uint8_t  password[8U];         ///< Password of association.
} csm_asso_config;

/**
 * @brief State and information of the currently open association
 */
typedef struct
{
    enum state_cf state_cf;
    const csm_asso_config *config;
} csm_asso_state;


void csm_asso_init(csm_asso_state *state);
int csm_asso_execute(csm_asso_state *state, csm_array *packet);


#endif // CSM_ASSOCIATION_H
