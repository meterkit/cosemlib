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

    csm_array_init(&array, (uint8_t*)&basic_array[0], size, size - 3U, 0U);

    TEST_ASSERT_EQUAL(size, array.size);

    uint32_t remaining = csm_array_unread(&array);
    TEST_ASSERT_EQUAL(7, remaining);

    for (uint32_t i = 0U; i < remaining; i++)
    {
        uint8_t byte;
        uint8_t ret = csm_array_get(&array, i, &byte);
        TEST_ASSERT_EQUAL(TRUE, ret);
        TEST_ASSERT_EQUAL(basic_array[i], byte);
    }

    // Now test with the offset parameter
    uint32_t offset = 4U;
    csm_array_init(&array, (uint8_t*)&basic_array[0], size, size - 3U, offset);

    remaining = csm_array_unread(&array);
    TEST_ASSERT_EQUAL(6, remaining);

    for (uint32_t i = 0U; i < remaining; i++)
    {
        uint8_t byte;
        uint8_t ret = csm_array_get(&array, i, &byte);
        TEST_ASSERT_EQUAL(TRUE, ret);
        TEST_ASSERT_EQUAL(basic_array[i+offset], byte);
    }

    //csm_array_dump(&array2);
}

// This test case tries to go out of bounds and see if everything's well protected
TEST(CosemArray, OverLimits)
{
    uint32_t size = sizeof(basic_array);
    csm_array array;

    csm_array_init(&array, (uint8_t*)&basic_array[0], size, size, 0U);


    // FIXME: test the array
}

TEST_GROUP_RUNNER(CosemArray)
{
  RUN_TEST_CASE(CosemArray, BasicTest);
  RUN_TEST_CASE(CosemArray, OverLimits);
}
