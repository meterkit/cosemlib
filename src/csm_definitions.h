#ifndef CSM_DEFINITIONS_H
#define CSM_DEFINITIONS_H

#include <stdint.h>
#include "csm_array.h"

#define CSM_DEF_LIB_VERSION         "1.0"
#define CSM_DEF_LLS_SIZE            8U
#define CSM_DEF_APP_TITLE_SIZE      8U
#define CSM_DEF_CHALLENGE_SIZE      64U
#define CSM_DEF_MAX_HLS_SIZE        (1U + 16U + CSM_DEF_CHALLENGE_SIZE) // SC + AK + Challenge

/**
 * @brief The xdlms_tag enum
 *
 *  GreenBook 8
 * 9.5       Abstract syntax of COSEM APDUs
 *
 */
enum xdlms_tag
{
    AXDR_INITIATE_REQUEST   = 1U,
    AXDR_INITIATE_RESPONSE  = 8U,
    AXDR_GET_REQUEST        = 192U,
    AXDR_SET_REQUEST        = 193U,
    AXDR_ACTION_REQUEST     = 195U,
    AXDR_GET_RESPONSE       = 196U,
    AXDR_SET_RESPONSE       = 197U,
    AXDR_ACTION_RESPONSE    = 199U,
    AXDR_EXCEPTION_RESPONSE = 216U
};

enum csm_conformance_mask
{
    /* -- the bit is set when the corresponding service or functionality is available
    reserved-zero                      (0),
    -- The actual list of general protection services depends on the security suite
    general-protection                 (1),
    general-block-transfer             (2),
    */
    CSM_CBLOCK_READ                             = 0x00100000U, // bit 3  - read
    CSM_CBLOCK_WRITE                            = 0x00080000U, // bit 4  - write
    CSM_CBLOCK_UNCONFIRMED_WRITE                = 0x00040000U, // bit 5  - unconfirmed-write
    CSM_CBLOCK_ATTRIBUTE0_SUPPORTED_WITH_SET    = 0x00008000U, // bit 8  - attribute0-supported-with-set
    CSM_CBLOCK_PRIORITY_MGT_SUPPORTED           = 0x00004000U, // bit 9  - priority-mgmt-supported
    CSM_CBLOCK_ATTRIBUTE0_SUPPORTED_WITH_GET    = 0x00002000U, // bit 10 - attribute0-supported-with-get
    CSM_CBLOCK_BLOCK_TRANSFER_WITH_GET_OR_READ  = 0x00001000U, // bit 11 - block-transfer-with-get-or-read
    CSM_CBLOCK_BLOCK_TRANSFER_WITH_SET_OR_WRITE = 0x00000800U, // bit 12 - block-transfer-with-set-or-write
    CSM_CBLOCK_BLOCK_TRANSFER_WITH_ACTION       = 0x00000400U, // bit 13 - block-transfer-with-action
    CSM_CBLOCK_MULTIPLE_REFERENCES              = 0x00000200U, // bit 14 - multiple-references
    CSM_CBLOCK_INFORMATION_REPORT               = 0x00000100U, // bit 15 - information-report
    CSM_CBLOCK_DATA_NOTIFICATION                = 0x00000080U, // bit 16 - data-notification
    CSM_CBLOCK_PARAMETERIZED_ACCESS             = 0x00000020U, // bit 18 - parameterized-access
    CSM_CBLOCK_GET                              = 0x00000010U, // bit 19 - get
    CSM_CBLOCK_SET                              = 0x00000008U, // bit 20 - set
    CSM_CBLOCK_SELECTIVE_ACCESS                 = 0x00000004U, // bit 21 - selective-access
    CSM_CBLOCK_EVENT_NOTIFICATION               = 0x00000002U, // bit 22 - event-notification
    CSM_CBLOCK_ACTION                           = 0x00000001U, // bit 23 - action
};


/**
 * \struct Obis
 */
typedef struct
{
    uint8_t    A; // Obis code field A
    uint8_t    B; // Obis code field B
    uint8_t    C; // Obis code field C
    uint8_t    D; // Obis code field D
    uint8_t    E; // Obis code field E
    uint8_t    F; // Obis code field F
} csm_obis_code;


typedef struct
{
    uint16_t        class_id;
    csm_obis_code   obis;
    uint8_t         version;
    int8_t          id;    //!< According to Blue Book 4.1.2 (Referencing method), negative values are private (app specific)
} csm_data;

typedef struct
{
    uint8_t use_sel_access; // 0 = FALSE, 1, TRUE
    uint8_t access_selector;
    csm_array *access_params;

} csm_selective_access;

typedef struct
{
    uint16_t ssap; //< Client
    uint16_t dsap; //< Server
} csm_llc;

enum csm_service { SRV_GET, SRV_SET, SRV_ACTION };

typedef struct
{
    enum csm_service service;
    csm_selective_access access;
    csm_data data;
    uint8_t db_type; //!< Database specific, base type of the data
} csm_db_request;

typedef struct
{
    csm_db_request db_request;
    uint8_t sender_invoke_id;
    uint8_t type; // Type of the request (normal, next ...)
    csm_llc llc;
    uint8_t channel_id; // Channel in use

} csm_request;

// ----------------------------- IMPLEMENTATION SPECIFIC INTERFACE -----------------------------

/* GreenBook 8
System title 4.3.4
The system title Sys-T shall  uniquely  identify  each DLMS/COSEM  entity  that may  be  server,  a client
or a third party that can access servers via clients. The system title:
  * shall be 8 octets long;
  * shall be unique.
The  leading  (i.e.,  the  3  leftmost)  octets  should  hold  the  three-letter  manufacturer  ID1.  This  is  the
same as the leading three octets of the Logical Device Name, see 4.3.5. The remaining 5 octets shall
ensure uniqueness.
*/

// This function makes a copy of the system title locally in RAM
void csm_sys_set_system_title(const uint8_t *buf);

// This function returns a pointer to the system title buffer string in memory
const uint8_t *csm_sys_get_system_title();

// Test the LLS password with the one given in reference
int csm_sys_test_lls_password(uint8_t sap, uint8_t *buf, uint32_t size);

// Generate a random number [0..255]
uint8_t csm_sys_get_random_u8();

// Get the mechanism_id of the specified association
uint8_t csm_sys_get_mechanism_id(uint8_t sap);

typedef enum
{
    CSM_SEC_KEK,    //!< the master key
    CSM_SEC_GUEK,   //!< global unicast encryption key GUEK
    CSM_SEC_GBEK,   //!< global broadcast encryption key GBEK
    CSM_SEC_GAK,    //!< (global) authentication key, GAK
} csm_sec_key;

uint8_t *csm_sys_get_key(uint8_t sap, csm_sec_key key_id);

int csm_sys_gcm_init(uint8_t channel, uint8_t sap, csm_sec_key key_id, const uint8_t *iv, const uint8_t *aad, uint32_t aad_len);
int csm_sys_gcm_update(uint8_t channel, const uint8_t *plain, uint32_t plain_len, uint8_t *crypt);
int csm_sys_gcm_finish(uint8_t channel, uint8_t *tag);


#endif // CSM_DEFINITIONS_H

