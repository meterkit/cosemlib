/**
 * Copyright (c) 2016, Anthony Rabine
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the BSD license.
 * See LICENSE.txt for more details.
 *
 * Implementation of the Cosem ACSE services
 */

#ifndef CSM_ASSOCIATION_H
#define CSM_ASSOCIATION_H

#include <stdint.h>
#include "csm_config.h"
#include "csm_ber.h"
#include "csm_definitions.h"

// States machine of the Control Function
enum state_cf { CF_INACTIVE, CF_IDLE, CF_ASSOCIATION_PENDING, CF_ASSOCIATED, CF_ASSOCIATION_RELEASE_PENDING };

/* GreenBook 8
 * 9.4.2.2.2        The COSEM application context
 * Table 74 – COSEM application context names

    Logical_Name_Referencing_No_Ciphering ::= context_id(1)
    Short_Name_Referencing_No_Ciphering ::= context_id(2)
    Logical_Name_Referencing_With_Ciphering ::= context_id(3)
    Short_Name_Referencing_With_Ciphering ::= context_id(4)
*/
enum csm_referencing
{
    NO_REF = 0U,
    LN_REF = 1U,  // Logical Name
    SN_REF = 2U,  // Short Name
    LN_REF_WITH_CYPHERING = 3U,
    SN_REF_WITH_CYPHERING = 4U
};

/*
    COSEM_lowest_level_security_mechanism_name ::= mechanism_id(0)
    COSEM_low_level_security_mechanism_name ::= mechanism_id(1)
    COSEM_high_level_security_mechanism_name ::= mechanism_id(2)
    COSEM_high_level_security_mechanism_name_using_MD5 ::= mechanism_id(3)
    COSEM_high_level_security_mechanism_name_using_SHA-1 ::= mechanism_id(4)
    COSEM_High_Level_Security_Mechanism_Name_Using_GMAC ::= mechanism_id(5)
    COSEM_High_Level_Security_Mechanism_Name_Using_SHA-256 ::= mechanism_id(6)
    COSEM_High_Level_Security_Mechanism_Name_Using_ECDSA ::= mechanism_id(7)
    NOTE 1 With mechanism_id(2), the method of processing the challenge is secret.
    NOTE 2 The use of authentication mechanisms 3 and 4 are not recommended for new implementations.
*/
enum csm_auth_level
{
    CSM_AUTH_LOWEST_LEVEL       = 0U,
    CSM_AUTH_LOW_LEVEL          = 1U,
    CSM_AUTH_HIGH_LEVEL_GMAC    = 5U
};

typedef enum
{
    CSM_ASSO_AARQ                   = TAG_APPLICATION + TAG_CONSTRUCTED + 0U,   ///< Application number 0
    CSM_ASSO_AARE                   = TAG_APPLICATION + TAG_CONSTRUCTED + 1U,   ///< Application number 1
    CSM_ASSO_RLRQ                   = TAG_APPLICATION + TAG_CONSTRUCTED + 2U,   ///< Application number 2
    CSM_ASSO_RLRE                   = TAG_APPLICATION + TAG_CONSTRUCTED + 3U,   ///< Application number 3s
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


enum csm_asso_error
{
    CSM_ASSO_ERR_NULL   = 0U,
    CSM_ASSO_ERR_AUTH_FAILURE = 13U
};

/**
 * @brief Configuration structure of one association
 */
typedef struct
{
    csm_llc llc;
    uint32_t conformance;          ///< All services and functionalities authorized.
    uint8_t  is_auto_connected;    ///< Boolean to indicate if the association is auto connected or not;
    uint8_t  authentication_mask;  ///< The mask of authentication authorized.
    uint8_t  password[SIZE_OF_AUTH_VALUE];         ///< Password of association.
} csm_asso_config;

/*
9.2.7.2.4.2         The security header
The security header SH includes the security control byte concatenated with the invocation counter:
SH = SC II IC. The security control byte is shown in Table 37 where:
•     Bit 3…0: Security_Suite_Id, see 9.2.3.7;
•     Bit 4: “A” subfield: indicates that authentication is applied;
•     Bit 5: “E” subfield: indicates that encryption is applied;
•     Bit 6: Key_Set subfield: 0 = Unicast, 1 = Broadcast;
•     Bit 7: Indicates the use of compression.
*/
typedef union
{
    uint8_t sh_byte;
    struct
    {
        uint8_t compression:1;
        uint8_t key_set:1;
        uint8_t encryption:1;
        uint8_t authentication:1;
        uint8_t security_suite:4;
    } sh_bit_field;
} csm_security_header;

/**
 * @brief State and information of the currently open association
 */
typedef struct
{
    enum state_cf state_cf;
    enum csm_referencing ref;
    enum csm_auth_level auth_level;
    enum csm_asso_error error;
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
