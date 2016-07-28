#ifndef CSM_DATABASE_H
#define CSM_DATABASE_H

#include <stdint.h>
#include "csm_services.h"

#define DB_ACCESS_GET       (uint16_t)1U
#define DB_ACCESS_SET       (uint16_t)2U
#define DB_ACCESS_GETSET    (uint16_t)3U


typedef enum
{
    DB_TYPE_NOT_USED = 0U,
    DB_TYPE_UNSIGNED8,
    DB_TYPE_UNSIGNED16,
    DB_TYPE_UNSIGNED32,
    DB_TYPE_UNSIGNED64,
    DB_TYPE_SIGNED8,
    DB_TYPE_SIGNED16,
    DB_TYPE_SIGNED32,
    DB_TYPE_SIGNED64,
    DB_TYPE_OCTET_STRING,
    DB_TYPE_DATE_TIME,
    DB_TYPE_DATE,
    DB_TYPE_TIME,
    DB_TYPE_ENUM,
    DB_TYPE_STRUCTURE,
    DB_TYPE_BIT_STRING,
    DB_TYPE_BOOLEAN,
    DB_TYPE_ARRAY,
    DB_TYPE_PROFILE_BUFFER,
    DB_TYPE_VISIBLE_STRING,
    DB_TYPE_ATTR_0,
    DB_TYPE_ATTR_1
} db_data_type;


typedef struct
{
     uint16_t  access_rights;   //!< Give the access right coding number
     int8_t    number;          //!<
     uint8_t   type;            //!< Gives the type of the attribute, for deserialization purpose
} db_attr_descr;

/**
 * @class CosemObject base object
 * @brief
**/
typedef struct
{
    const db_attr_descr *attr_list;  ///< The table of attributes
    const db_attr_descr *meth_list;  ///< The table of methods
    uint16_t  class_id;       ///< Class Id
    csm_obis_code   obis_code;          ///< Obis code for Logical Name
    uint8_t   version;       ///< Version
    uint8_t   nb_attr;  ///< Number of attributes
    uint8_t   nb_meth;  ///< Number of methods

} db_object_descr;


typedef struct
{
    const db_object_descr *objects;
    csm_db_access_handler handler;
    uint8_t nb_objects;
} db_element;

/**
 * @brief Handle pointing to a ROM Cosem object
 */
typedef struct
{
    const db_object_descr *object; ///< Pointer to the cosem object
    uint8_t db_index; // database number (index)
    uint8_t obj_index; // object index in the database
} db_obj_handle;


// Database access from Cosem
csm_db_code csm_db_access_func(csm_array *in, csm_array *out, csm_request *request);


#endif // CSM_DATABASE_H
