#include "csm_system.h"
#include "os.h"
#include <string.h>

/*
the  leading  (i.e.  the  leftmost)  64  bits  (8  octets)  shall  hold  the  fixed  field.  It  shall  contain  the
system title, see 4.3.4;
•     the trailing (i.e. the rightmost) 32 bits shall hold the invocation field. The invocation field shall be
an integer counter.
*/

static uint8_t initialization_vector[12] = { 0x4DU, 0x4DU, 0x4DU, 0x00U, 0x00U, 0xBCU, 0x61U, 0x4EU,  // system title
                                             0U, 0U, 0U, 0U }; // counter

static const uint8_t key_kek[16] = { 0xFFU,0xFFU,0xFFU,0xFFU,0xFFU,0xFFU,0xFFU,0xFFU,0xFFU,0xFFU,0xFFU,0xFFU,0xFFU,0xFFU,0xFFU,0xFFU };
static const uint8_t key_guek[16] = { 0xFFU,0xFFU,0xFFU,0xFFU,0xFFU,0xFFU,0xFFU,0xFFU,0xFFU,0xFFU,0xFFU,0xFFU,0xFFU,0xFFU,0xFFU,0xFFU };
static const uint8_t key_gbek[16] = { 0xFFU,0xFFU,0xFFU,0xFFU,0xFFU,0xFFU,0xFFU,0xFFU,0xFFU,0xFFU,0xFFU,0xFFU,0xFFU,0xFFU,0xFFU,0xFFU };
static const uint8_t key_gak[16] = { 0xFFU,0xFFU,0xFFU,0xFFU,0xFFU,0xFFU,0xFFU,0xFFU,0xFFU,0xFFU,0xFFU,0xFFU,0xFFU,0xFFU,0xFFU,0xFFU };

void csm_sys_set_system_title(const uint8_t *buf)
{
    memcpy(initialization_vector, buf, 8);
}

void csm_sys_get_system_title(uint8_t *buf)
{
    memcpy(buf, initialization_vector, 8);
}

void csm_sys_get_init_vector(uint8_t *buf, uint32_t counter)
{
    PUT_BE32(&initialization_vector[8], counter);
    memcpy(buf, initialization_vector, 12);
}

void csm_sys_get_key(enum csm_key key, uint8_t *buf)
{
    const uint8_t *from = NULL;

    switch(key)
    {
    case KEK:
        from = key_kek;
        break;
    case GUEK:
        from = key_guek;
        break;
    case GBEK:
        from = key_gbek;
        break;
    default:
    case GAK:
        from = key_gak;
        break;
    }

    memcpy(buf, from, 16);
}
