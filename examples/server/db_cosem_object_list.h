#include "app_database.h"
#include "db_cosem_clock.h"
#include "db_cosem_associations.h"

const db_attr_descr clock_attributes[] = {
{ DB_ACCESS_GETSET, 2, DB_TYPE_DATE_TIME },
{ DB_ACCESS_GETSET, 3, DB_TYPE_SIGNED16 },
{ DB_ACCESS_GET,     4, DB_TYPE_UNSIGNED8 },
{ DB_ACCESS_GETSET, 5, DB_TYPE_DATE_TIME },
{ DB_ACCESS_GETSET, 6, DB_TYPE_DATE_TIME },
{ DB_ACCESS_GETSET, 7, DB_TYPE_SIGNED8 },
{ DB_ACCESS_GETSET, 8, DB_TYPE_BOOLEAN },
{ DB_ACCESS_GETSET, 9, DB_TYPE_ENUM },
};

const db_attr_descr association1_attributes[] = {
{ DB_ACCESS_GET, 2, DB_TYPE_ARRAY },
{ DB_ACCESS_GET, 3, DB_TYPE_STRUCTURE },
{ DB_ACCESS_GET, 4, DB_TYPE_STRUCTURE },
{ DB_ACCESS_GET, 5, DB_TYPE_STRUCTURE },
{ DB_ACCESS_GET, 6, DB_TYPE_STRUCTURE },
{ DB_ACCESS_GETSET, 8, DB_TYPE_ENUM },
{ DB_ACCESS_GETSET, 9, DB_TYPE_OCTET_STRING },
};

const db_object_descr clock_objects[] = {
    {&clock_attributes[0], NULL, 8U , { 0U, 0U, 1U, 0U, 0U, 255U } , 0U , 8U, 0U },
};

const db_object_descr associations_objects[] = {
    {&association1_attributes[0], NULL, 15U , { 0U, 0U, 40U, 0U, 0U, 255U } , 0U , 7U, 0U },
};

const db_element gDataBaseList[] = {
    { &clock_objects[0], db_cosem_clock_func, 1U },
    { &associations_objects[0], db_cosem_associations_func, 1U },
};


#define NUMBER_OF_OBJECTS   2U
#define NUMBER_OF_DATABASES (sizeof(gDataBaseList)/sizeof(db_element))

