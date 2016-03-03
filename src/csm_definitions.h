#ifndef CSM_DEFINITIONS_H
#define CSM_DEFINITIONS_H

#include <stdint.h>
#include "csm_array.h"

enum csm_conformance_mask
{
    /* -- the bit is set when the corresponding service or functionality is available
    reserved-zero                      (0),
    -- The actual list of general protection services depends on the security suite
    general-protection                 (1),
    general-block-transfer             (2),
    */
    CSM_READ                             = 0x00100000U, // bit 3  - read
    CSM_WRITE                            = 0x00080000U, // bit 4  - write
    CSM_UNCONFIRMED_WRITE                = 0x00040000U, // bit 5  - unconfirmed-write
    CSM_ATTRIBUTE0_SUPPORTED_WITH_SET    = 0x00008000U, // bit 8  - attribute0-supported-with-set
    CSM_PRIORITY_MGT_SUPPORTED           = 0x00004000U, // bit 9  - priority-mgmt-supported
    CSM_ATTRIBUTE0_SUPPORTED_WITH_GET    = 0x00002000U, // bit 10 - attribute0-supported-with-get
    CSM_BLOCK_TRANSFER_WITH_GET_OR_READ  = 0x00001000U, // bit 11 - block-transfer-with-get-or-read
    CSM_BLOCK_TRANSFER_WITH_SET_OR_WRITE = 0x00000800U, // bit 12 - block-transfer-with-set-or-write
    CSM_BLOCK_TRANSFER_WITH_ACTION       = 0x00000400U, // bit 13 - block-transfer-with-action
    CSM_MULTIPLE_REFERENCES              = 0x00000200U, // bit 14 - multiple-references
    CSM_INFORMATION_REPORT               = 0x00000100U, // bit 15 - information-report
    CSM_DATA_NOTIFICATION                = 0x00000080U, // bit 16 - data-notification
    CSM_PARAMETERIZED_ACCESS             = 0x00000020U, // bit 18 - parameterized-access
    CSM_GET                              = 0x00000010U, // bit 19 - get
    CSM_SET                              = 0x00000008U, // bit 20 - set
    CSM_SELECTIVE_ACCESS                 = 0x00000004U, // bit 21 - selective-access
    CSM_EVENT_NOTIFICATION               = 0x00000002U, // bit 22 - event-notification
    CSM_ACTION                           = 0x00000001U, // bit 23 - action
};


enum csm_auth_level
{
    CSM_LOWEST_AUTHENTICATION           = 0x00U,
    CSM_LOW_AUTHENTICATION              = 0x01U,
    CSM_HIGH_AUTHENTICATION             = 0x02U,
    CSM_HIGH_AUTHENTICATION_WITH_MD5    = 0x03U,
    CSM_HIGH_AUTHENTICATION_WITH_SHA1   = 0x04U,
    CSM_HIGH_AUTHENTICATION_WITH_GMAC   = 0x05U
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


/**
 * @brief attribute
 */
typedef struct
{
    uint8_t    is_method;       ///< 0 = attribute, 1 = method
    int8_t     attribute_id;    ///< According to Blue Book 4.1.2 (Referencing method), negative values are possible
} csm_attr;


typedef struct
{
    csm_obis_code   obis;
    uint16_t        class_id;
    csm_attr        attribute;
    uint8_t         version;
} csm_object;

typedef struct
{
    uint8_t use_sel_access; // 0 = FALSE, 1, TRUE
    uint8_t access_selector;
    csm_array *access_params;

} csm_selective_access;

/**
 * @brief Generic Codec error codes
 **/
typedef enum
{
    CSM_OK,            ///< All is OK
    CSM_ERR_BAD_ENCODING,        ///< Bad encoding of codec
    CSM_ERR_OBJECT_ERROR,        ///< Generic error coming from the object
    CSM_ERR_UNAUTHORIZED_ACCESS, ///< Service is unauthorized.
    CSM_ERR_READ_WRITE_DENIED,   ///< Data access is unauthorized.
    CSM_ERR_DATA_BAD_ENCODING,   ///< Bad encoding of data.
    CSM_ERR_TEMPORARY_FAILURE,   ///< Temporary failure.
    CSM_ERR_DATA_UNDEFINED,      ///< Data is undefined.
    CSM_ERR_DATA_CONTENT_NOT_OK, ///< Data content is not accepted.
    CSM_ERR_APDU_BUFFER_FULL,    ///< Apdu is full.
    CSM_ERR_FRAGMENTATION_USED   ///< Fragmentation used, request not completly performed
} csm_db_code;

typedef csm_db_code (*extract_data)(csm_array *array, const csm_object *object, const csm_selective_access *sel_access);
typedef csm_db_code (*insert_data)(csm_array *array, const csm_object *object, const csm_selective_access *sel_access);


typedef struct
{
    extract_data extract_func;
    insert_data insert_func;
} csm_db_access;


#endif // CSM_DEFINITIONS_H

