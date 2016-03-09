
#include "csm_db_gen.h"
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



csm_obj_handle csm_db_get_obj_from_obis(const csm_obis_code *obis, uint16_t class_id)
{
    csm_obj_handle handle;
    uint8_t end_loop = FALSE;

    for (uint8_t i = 0U; (i < NUMBER_OF_DATABASES) && (!end_loop); i++)
    {
        const csm_db_list *obj_list = &gDataBaseList[i];

        // Loop on all cosem object in list
        for (uint8_t object_index = 0U; (object_index < obj_list->nb_objects) && (!end_loop); object_index++)
        {
            const csm_object_descr *curr_obj = &obj_list->objects[object_index];
            // Check the obis code
            if ((curr_obj->class_id == class_id) && csm_is_obis_equal(&curr_obj->obis_code, obis))
            {
                handle.object       = curr_obj;
                handle.db_index      = i;
                handle.obj_index  = object_index;
                end_loop         = TRUE;
            }
        }
    }

    return handle;
}


csm_db_code csm_db_extract_data(csm_array *array, const csm_object *object, const csm_selective_access *sel_access)
{
    (void) array;
    (void) object;
    (void) sel_access;
    return CSM_ERR_OBJECT_ERROR;
}

csm_db_code csm_db_insert_data(csm_array *array, const csm_object *object, const csm_selective_access *sel_access)
{
    (void) array;
    (void) object;
    (void) sel_access;
    return CSM_ERR_OBJECT_ERROR;
}
