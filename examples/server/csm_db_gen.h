#include "csm_database.h"

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

const csm_attr_descr ClockAttributes[] = {
{ 0x0002U, CSM_GET_SET, { 0U, 2 }, CSM_TYPE_DATE_TIME, 0U },
{ 0x0043U, CSM_GET_SET, { 0U, 3 }, CSM_TYPE_SIGNED16, 0U },
{ 0x0084U, CSM_GET,     { 0U, 4 }, CSM_TYPE_UNSIGNED8, 0U },
{ 0x00C5U, CSM_GET_SET, { 0U, 5 }, CSM_TYPE_DATE_TIME, 0U },
{ 0x0106U, CSM_GET_SET, { 0U, 6 }, CSM_TYPE_DATE_TIME, 0U },
{ 0x0147U, CSM_GET_SET, { 0U, 7 }, CSM_TYPE_SIGNED8, 0U },
{ 0x0188U, CSM_GET_SET, { 0U, 8 }, CSM_TYPE_BOOLEAN, 0U },
{ 0x01C9U, CSM_GET_SET, { 0U, 9 }, CSM_TYPE_ENUM, 0U },
};

const csm_object_descr EventManagerObjects[] = {
	{&ApplicationEventLogAttributes[0] , 7U , { 1U, 2U, 99U, 98U, 0U, 255U } , 1U , 8U }, 
};

const csm_object_descr ClockObjects[] = {
	{&ClockAttributes[0] , 8U , { 0U, 0U, 1U, 0U, 0U, 255U } , 0U , 8U }, 
};

const csm_db_list gDataBaseList[] = {
{ &EventManagerObjects[0], 29U, 1U },
{ &ClockObjects[0], 5U, 1U },
};


#define NUMBER_OF_OBJECTS   2U
#define NUMBER_OF_DATABASES (sizeof(gDataBaseList)/sizeof(csm_db_list))

