#ifndef CSM_SERVICES_H
#define CSM_SERVICES_H

#include "csm_definitions.h"

void csm_services_init(csm_db_access *db_access);
int csm_services_execute(csm_array *array);

#endif // CSM_SERVICES_H
