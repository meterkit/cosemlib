#ifndef CSM_DATABASE_H
#define CSM_DATABASE_H

#include <stdint.h>
#include "csm_definitions.h"

#define CSM_NO_ACCESS   (uint16_t)0U
#define CSM_GET         (uint16_t)1U
#define CSM_SET         (uint16_t)2U
#define CSM_GET_SET     (uint16_t)3U


typedef enum
{
    CSM_TYPE_NOT_USED = 0U,
    CSM_TYPE_UNSIGNED8,
    CSM_TYPE_UNSIGNED16,
    CSM_TYPE_UNSIGNED32,
    CSM_TYPE_UNSIGNED64,
    CSM_TYPE_SIGNED8,
    CSM_TYPE_SIGNED16,
    CSM_TYPE_SIGNED32,
    CSM_TYPE_SIGNED64,
    CSM_TYPE_OCTET_STRING,
    CSM_TYPE_DATE_TIME,
    CSM_TYPE_DATE,
    CSM_TYPE_TIME,
    CSM_TYPE_ENUM,
    CSM_TYPE_STRUCTURE,
    CSM_TYPE_BIT_STRING,
    CSM_TYPE_BOOLEAN,
    CSM_TYPE_ARRAY,
    CSM_TYPE_PROFILE_BUFFER,
    CSM_TYPE_VISIBLE_STRING,
    CSM_TYPE_ATTR_0,
    CSM_TYPE_ATTR_1
} csm_data_type;


typedef struct
{
     uint16_t  data_id;         //< Unique nale within a database &
     uint16_t  access_rights;   //< Give the access right coding number
     csm_attr  attribute;       //<
     uint8_t   type;            //< Gives the type of the attribute, for deserialization purpose
     uint8_t   cat_id;        //< Category of this attribute, can be used freely by the application
} csm_attr_descr;

/**
 * @class CosemObject base object
 * @brief
**/
typedef struct
{
    const csm_attr_descr *attr_list;  ///< The table of attributes.
    uint16_t  class_id;       ///< Class Id
    csm_obis_code   obis_code;          ///< Obis code for Logical Name
    uint8_t   version;       ///< Version
    uint8_t   nb_attr;  ///< Number of attributes AND methods

} csm_object_descr;

typedef struct
{
    const csm_object_descr *objects;
    uint8_t db_id;
    uint8_t nb_objects;
} csm_db_list;

/**
 * @brief Handle pointing to a ROM Cosem object
 */
typedef struct
{
    const csm_object_descr *object; ///< Pointer to the cosem object
    uint8_t db_index; // index in the database
    uint8_t obj_index; // index of the object in the database
} csm_obj_handle;


csm_obj_handle csm_db_get_obj_from_obis(const csm_obis_code *obis, uint16_t class_id);

#endif // CSM_DATABASE_H
