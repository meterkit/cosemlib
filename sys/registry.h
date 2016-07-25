#ifndef REGISTRY_H
#define REGISTRY_H


#include "stdint.h"

// Argument is project specific
typedef int (*sys_handler)(uint8_t *arg, uint32_t size);

// Definition of a software component
typedef struct
{
    sys_handler initialize;
    sys_handler start;
    sys_handler suspend;
    sys_handler resume;
    sys_handler stop;
} sys_component;

#endif // REGISTRY_H
