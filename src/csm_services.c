#include "csm_services.h"

static csm_db_access *database = NULL;

void csm_services_init(csm_db_access *db_access)
{
    database = db_access;
}

int csm_services_execute(csm_array *array)
{
    if (database != NULL)
    {
        // FIXME: filter the service type, then call it
    }
}
