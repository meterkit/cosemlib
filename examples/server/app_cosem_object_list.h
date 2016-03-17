#include "app_database.h"
#include "app_cosem_clock.h"

/*
const csm_attr_descr ApplicationEventLogAttributes[] = {
{ 0x0002U, CSM_GET, { 0U, 2 }, CSM_TYPE_PROFILE_BUFFER, 0U },
{ 0x0043U, CSM_GET, { 0U, 3 }, CSM_TYPE_ARRAY, 0U },
{ 0x0084U, CSM_GET, { 0U, 4 }, CSM_TYPE_UNSIGNED32, 0U },
{ 0x00C5U, CSM_GET, { 0U, 5 }, CSM_TYPE_ENUM, 0U },
{ 0x0106U, CSM_GET, { 0U, 6 }, CSM_TYPE_STRUCTURE, 0U },
{ 0x0147U, CSM_GET, { 0U, 7 }, CSM_TYPE_UNSIGNED32, 0U },
{ 0x0188U, CSM_GET, { 0U, 8 }, CSM_TYPE_UNSIGNED32, 0U },
{ 0x01E1U, CSM_GET_SET, { 1U, 1 }, CSM_TYPE_ENUM, 0U },
};
*/
const csm_attr_descr clock_attributes[] = {
{ CSM_GET_SET, 2, CSM_TYPE_DATE_TIME },
{ CSM_GET_SET, 3, CSM_TYPE_SIGNED16 },
{ CSM_GET,     4, CSM_TYPE_UNSIGNED8 },
{ CSM_GET_SET, 5, CSM_TYPE_DATE_TIME },
{ CSM_GET_SET, 6, CSM_TYPE_DATE_TIME },
{ CSM_GET_SET, 7, CSM_TYPE_SIGNED8 },
{ CSM_GET_SET, 8, CSM_TYPE_BOOLEAN },
{ CSM_GET_SET, 9, CSM_TYPE_ENUM },
};

/*
const csm_object_descr EventManagerObjects[] = {
	{&ApplicationEventLogAttributes[0] , 7U , { 1U, 2U, 99U, 98U, 0U, 255U } , 1U , 8U }, 
};
*/

const csm_object_descr clock_objects[] = {
    {&clock_attributes[0], NULL, 8U , { 0U, 0U, 1U, 0U, 0U, 255U } , 0U , 8U, 0U },
};

const csm_db_element gDataBaseList[] = {
//{ &EventManagerObjects[0], 1U },
{ &clock_objects[0], csm_db_clock_func, 1U },
};


#define NUMBER_OF_OBJECTS   1U
#define NUMBER_OF_DATABASES (sizeof(gDataBaseList)/sizeof(csm_db_element))

