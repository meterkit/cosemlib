
#include <stdlib.h>
#include <string.h>

#include "unity_fixture.h"
#include "csm_array.h"
#include "csm_ber.h"

/*
static void RunAllTests(void)
{
  RUN_TEST_GROUP(CosemArray);
}
*/

static const uint8_t association[] = { 0x60U,  0x36U, 0xA1U,   0x09U,   0x06U,   0x07U,  0x60U,  0x85U,
0x74U,   0x05U,   0x08U,   0x01U,   0x01U, 0x8AU,   0x02U,   0x07U,
0x80U, 0x8BU,   0x07U,  0x60U,  0x85U,  0x74U,   0x05U,   0x08U,
 0x02U,   0x01U, 0xACU, 0x0AU,  0x80U,   0x08U,  0x41U,  0x42U,
0x43U,  0x44U,  0x45U,  0x46U,  0x47U,  0x48U, 0xBEU,  0x10U,
 0x04U, 0x0EU,   0x01U,   0x00U,   0x00U,   0x00U,   0x06U, 0x5FU,
0x1FU,   0x04U,   0x00U,   0x00U,  0x30U, 0x1DU, 0xFFU, 0xFFU };


int main(int argc, const char * argv[])
{

  //return UnityMain(argc, argv, RunAllTests);

    (void) argc;
    (void) argv;

    uint32_t size = sizeof(association);

    // Intermediate buffer
    uint8_t buffer[size];
    memcpy(buffer, association, size);

    csm_array array;
    csm_array_alloc(&array, &buffer[0], size);

    csm_ber ber;
    csm_ber_read_tag(&array, &ber.tag);
    csm_ber_read_len(&array, &ber.length);

    csm_ber_dump(&ber);
    //csm_array_dump(&array2);

}

