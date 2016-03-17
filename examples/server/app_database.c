
#include "app_cosem_object_list.h"
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



int csm_db_get_object(const csm_data *data, csm_obj_handle *handle)
{

    uint8_t found = FALSE;

    for (uint8_t i = 0U; (i < NUMBER_OF_DATABASES) && (!found); i++)
    {
        const csm_db_element *obj_list = &gDataBaseList[i];

        // Loop on all cosem object in list
        for (uint8_t object_index = 0U; (object_index < obj_list->nb_objects) && (!found); object_index++)
        {
            const csm_object_descr *curr_obj = &obj_list->objects[object_index];
            // Check the obis code
            if ((curr_obj->class_id == data->class_id) && csm_is_obis_equal(&curr_obj->obis_code, &data->obis))
            {
                handle->object       = curr_obj;
                handle->db_index     = i;
                handle->obj_index    = object_index;
                found            = TRUE;
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

    csm_obj_handle handle;

    if (csm_db_get_object(&request->db_request.data, &handle))
    {
        // Ok, call the database main function
        const csm_db_element *db_element = &gDataBaseList[handle.db_index];
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

