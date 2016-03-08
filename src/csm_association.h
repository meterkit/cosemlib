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

enum referencing
{
    NO_REF = 0U,
    LN_REF = 1U,  // Logical Name
    SN_REF = 2U   // Short Name
};

enum auth_level
{
    NO_LEVEL    = 0U,
    LOW_LEVEL   = 1U,
    HIGH_LEVEL  = 5U
};

typedef enum
{
    CSM_ASSO_AARQ                   = TAG_APPLICATION + TAG_CONSTRUCTED + 0U,   ///< Application number 0
    CSM_ASSO_AARE                   = TAG_APPLICATION + TAG_CONSTRUCTED + 1U,   ///< Application number 1
    CSM_ASSO_PROTO_VER              = TAG_CONTEXT_SPECIFIC + TAG_PRIMITIVE,
    CSM_ASSO_APP_CONTEXT_NAME       = TAG_CONTEXT_SPECIFIC + TAG_CONSTRUCTED +  1U,

    // AARQ tags
    CSM_ASSO_CALLED_AP_TITLE        = TAG_CONTEXT_SPECIFIC + TAG_CONSTRUCTED +  2U,
    CSM_ASSO_CALLED_AE_QUALIFIER    = TAG_CONTEXT_SPECIFIC + TAG_CONSTRUCTED +  3U,
    CSM_ASSO_CALLED_AP_INVOC_ID     = TAG_CONTEXT_SPECIFIC + TAG_CONSTRUCTED +  4U,
    CSM_ASSO_CALLED_AE_INVOC_ID     = TAG_CONTEXT_SPECIFIC + TAG_CONSTRUCTED +  5U,
    CSM_ASSO_CALLING_AP_TITLE       = TAG_CONTEXT_SPECIFIC + TAG_CONSTRUCTED +  6U,
    CSM_ASSO_CALLING_AE_QUALIFIER   = TAG_CONTEXT_SPECIFIC + TAG_CONSTRUCTED +  7U,
    CSM_ASSO_CALLING_AP_INVOC_ID    = TAG_CONTEXT_SPECIFIC + TAG_CONSTRUCTED +  8U,
    CSM_ASSO_CALLING_AE_INVOC_ID    = TAG_CONTEXT_SPECIFIC + TAG_CONSTRUCTED +  9U,
    CSM_ASSO_SENDER_ACSE_REQU       = TAG_CONTEXT_SPECIFIC + TAG_PRIMITIVE   + 10U,
    CSM_ASSO_MECHANISM_NAME         = TAG_CONTEXT_SPECIFIC + TAG_PRIMITIVE   + 11U,
    CSM_ASSO_CALLING_AUTH_VALUE     = TAG_CONTEXT_SPECIFIC + TAG_CONSTRUCTED + 12U,
    CSM_ASSO_IMPLEMENTATION_INFO    = TAG_CONTEXT_SPECIFIC + TAG_CONSTRUCTED + 29U,
    CSM_ASSO_USER_INFORMATION       = TAG_CONTEXT_SPECIFIC + TAG_CONSTRUCTED + 30U,

    // AARE tags
    CSM_ASSO_RESULT_FIELD           = TAG_CONTEXT_SPECIFIC + TAG_CONSTRUCTED +  2U,
    CSM_ASSO_RESULT_SRC_DIAG        = TAG_CONTEXT_SPECIFIC + TAG_CONSTRUCTED +  3U,
    CSM_ASSO_RESULT_SERVICE_USER    = TAG_CONTEXT_SPECIFIC + TAG_CONSTRUCTED +  1U,
} csm_asso_tag;


enum asso_error
{
    CSM_ASSO_ERR_NULL   = 0U,
    CSM_ASSO_ERR_AUTH_FAILURE = 13U
};

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
    uint8_t  password[SIZE_OF_AUTH_VALUE];         ///< Password of association.
} csm_asso_config;

/**
 * @brief State and information of the currently open association
 */
typedef struct
{
    enum state_cf state_cf;
    enum referencing ref;
    enum auth_level auth_level;
    enum asso_error error;
    uint8_t  auth_value[SIZE_OF_AUTH_VALUE];
    uint8_t dedicated_key[SIZE_OF_DEDICATED_KEY];
    uint32_t proposed_conformance;
    uint16_t client_max_receive_pdu_size;

    // Pointer to the configuration structure
    const csm_asso_config *config;
} csm_asso_state;


void csm_asso_init(csm_asso_state *state);
int csm_asso_execute(csm_asso_state *state, csm_array *packet);


#endif // CSM_ASSOCIATION_H
