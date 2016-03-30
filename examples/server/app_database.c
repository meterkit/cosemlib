
#include "db_cosem_object_list.h"
#include "csm_config.h"


uint8_t csm_is_obis_equal(const csm_obis_code *first, const csm_obis_code *second)
{
    uint8_t ret = FALSE;
    if ((first->A == second->A) &&
        (first->B == second->B) &&
        (first->C == second->C) &&
        (first->D == second->D) &&
        (first->E == second->E) &&
        (first->F == second->F))
    {
        ret = TRUE;
    }

    return ret;
}

static int csm_db_check_attribute(csm_db_request *db_request, const db_object_descr *object)
{
    int ret = FALSE;
    const db_attr_descr *attr = NULL;
    uint8_t access_rights = 0U;
    uint8_t size = 0U;

    if (db_request->service == SRV_GET)
    {
        access_rights |= DB_ACCESS_GET;
        attr = object->attr_list;
        size = object->nb_attr;
    }
    if (db_request->service == SRV_SET)
    {
        access_rights |= DB_ACCESS_SET;
        attr = object->attr_list;
        size = object->nb_attr;
    }
    if (db_request->service == SRV_ACTION)
    {
        access_rights |= DB_ACCESS_GETSET;
        attr = object->meth_list;
        size = object->nb_meth;
    }

    if (attr != NULL)
    {
        for (uint8_t i = 0U; i < size; i++)
        {
            // Check attribute number and access rights
            if ((attr->number == db_request->data.id) && (access_rights & attr->access_rights) == access_rights)
            {
                // Save the type for application usage
                db_request->db_type = attr->type;
                ret = TRUE;
                break;
            }
        }
    }

    return ret;
}


static int csm_db_get_object(csm_db_request *db_request, db_obj_handle *handle)
{
    uint8_t found = FALSE;

    for (uint8_t i = 0U; (i < NUMBER_OF_DATABASES) && (!found); i++)
    {
        const db_element *obj_list = &gDataBaseList[i];

        // Loop on all cosem object in list
        for (uint8_t object_index = 0U; (object_index < obj_list->nb_objects) && (!found); object_index++)
        {
            const db_object_descr *curr_obj = &obj_list->objects[object_index];
            // Check the obis code
            if ((curr_obj->class_id == db_request->data.class_id) && csm_is_obis_equal(&curr_obj->obis_code, &db_request->data.obis))
            {
                handle->object       = curr_obj;
                handle->db_index     = i;
                handle->obj_index    = object_index;

                // Verify that this object contains the suitable method/attribute and if we can access to it
                found = csm_db_check_attribute(db_request, curr_obj);
            }
        }
    }

    return found;
}


csm_db_code csm_db_access_func(csm_array *array, csm_request *request)
{
    csm_db_code code = CSM_ERR_OBJECT_ERROR;
    // We want to access to an object. First, gets an handle to its parameters
    // If the handle retrieval fails, then this cosem object probably not exists

    db_obj_handle handle;

    if (csm_db_get_object(&request->db_request, &handle))
    {
        // Ok, call the database main function
        const db_element *db_element = &gDataBaseList[handle.db_index];
        if (db_element->handler != NULL)
        {
            code = db_element->handler(array, request);
        }
        else
        {
            CSM_ERR("[DB] Cannot access to DB handler function");
        }
    }
    else
    {
        CSM_ERR("[DB] Cosem object not found");
        code = CSM_ERR_OBJECT_NOT_FOUND;
    }

    return code;
}

