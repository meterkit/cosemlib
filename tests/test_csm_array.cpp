
extern "C"
{
    #include "csm_array.h"
}
#include "catch.hpp"

static const uint8_t basic_array[10] = {1U, 2U, 3U, 4U, 5U, 6U, 7U, 8U, 9U, 10U};


void csm_array_basic_test(void)
{
    uint32_t size = sizeof(basic_array);
    csm_array array;

    csm_array_init(&array, (uint8_t*)&basic_array[0], size, size - 3U, 0U);

    REQUIRE(size == array.size);

    uint32_t remaining = csm_array_unread(&array);
    REQUIRE(7 == remaining);

    for (uint32_t i = 0U; i < remaining; i++)
    {
        uint8_t byte;
        uint8_t ret = csm_array_get(&array, i, &byte);
        REQUIRE(TRUE == ret);
        REQUIRE(basic_array[i] == byte);
    }

    // Now test with the offset parameter
    uint32_t offset = 4U;
    csm_array_init(&array, (uint8_t*)&basic_array[0], size, size - 3U, offset);

    remaining = csm_array_unread(&array);
    REQUIRE(6 == remaining);

    for (uint32_t i = 0U; i < remaining; i++)
    {
        uint8_t byte;
        uint8_t ret = csm_array_get(&array, i, &byte);
        REQUIRE(TRUE == ret);
        REQUIRE(basic_array[i+offset] == byte);
    }

    //csm_array_dump(&array2);
}

// This test case tries to go out of bounds and see if everything's well protected
void over_limits(void)
{
    uint32_t size = sizeof(basic_array);
    csm_array array;

    csm_array_init(&array, (uint8_t*)&basic_array[0], size, size, 0U);


    // FIXME: test the array
}


TEST_CASE( "Cosem: array utility tests", "[csm_array_tests]" )
{
    csm_array_basic_test();
    over_limits();
}


