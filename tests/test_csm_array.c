#include "csm_array.h"
#include "unity.h"
#include "unity_fixture.h"

static const uint8_t basic_array[10] = {1U, 2U, 3U, 4U, 5U, 6U, 7U, 8U, 9U, 10U};

TEST_GROUP(CosemArray);


TEST_SETUP(CosemArray)
{
}

TEST_TEAR_DOWN(CosemArray)
{
}

TEST(CosemArray, BasicTest)
{
    uint32_t size = sizeof(basic_array);
    csm_array array;
    csm_array array2;

    csm_array_init(&array, (uint8_t*)&basic_array[0], size, size);

    TEST_ASSERT_EQUAL(size, array.size);

    // Test sub array access
    csm_array_mid(&array, &array2, 2, 5);

    //csm_array_dump(&array2);
    // FIXME: test the array
}

// This test case tries to go out of bounds and see if everything's well protected
TEST(CosemArray, OverLimits)
{
    uint32_t size = sizeof(basic_array);
    csm_array array;
    csm_array array2;
    csm_array_init(&array, (uint8_t*)&basic_array[0], size, size);

    csm_array_mid(&array, &array2, 6, 8);
    //csm_array_dump(&array2);

    // FIXME: test the array
}

TEST_GROUP_RUNNER(CosemArray)
{
  RUN_TEST_CASE(CosemArray, BasicTest);
  RUN_TEST_CASE(CosemArray, OverLimits);
}
