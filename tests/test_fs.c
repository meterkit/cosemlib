#include "fs.h"
#include "unity.h"
#include "unity_fixture.h"

typedef struct
{
    uint32_t data[20];  // registers
    uint16_t code; // event code
    uint8_t date_time[12]; // cosem date-time format
    uint8_t status; // AMR status

} cfg_test_sample_log;


typedef struct
{
    uint8_t data1;
    uint32_t data2[4];

} cfg_test_tiny_file;

static const cfg_test_tiny_file gDefault = { 0U, { 0U, 0U, 0U, 0U } };

#define GENERAL_EVENT_LOG_ENTRIES   100

typedef enum {
    FS_FILE_1,
    FS_FILE_2,
    FS_FILE_3

} fs_fils_id;

static const fs_file_cfg cFiles[] = {
    { FS_FILE_1, 'S', 0U, 2U, sizeof(cfg_test_tiny_file), &gDefault },
    { FS_FILE_2, 'L', 0U, 2U, GENERAL_EVENT_LOG_ENTRIES*sizeof(cfg_test_sample_log), NULL }, // data file, no any default contents
};

#define NB_FILES (sizeof(cFiles) / sizeof(&cFiles[0]))


TEST_GROUP(FileSystem);


TEST_SETUP(FileSystem)
{
    // FIXME: setup flash memory initial conditions
    // - FS is formatted (every byte is at 0xFF)
}

TEST_TEAR_DOWN(FileSystem)
{
}

TEST(FileSystem, Open)
{
    fs_handle handle;

    fs_initialize(&cFiles[0], NB_FILES);

    int ret = fs_open(&handle, FS_FILE_3);

    TEST_ASSERT_EQUAL(FS_FILE_NOT_FOUND, ret);


}

TEST_GROUP_RUNNER(FileSystem)
{
  RUN_TEST_CASE(FileSystem, Open);
}
