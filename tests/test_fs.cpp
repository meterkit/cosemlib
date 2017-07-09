#include "fs.h"
#include "embUnit.h"

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

#define TINY_FILE_SIZE  sizeof(cfg_test_tiny_file)

static const cfg_test_tiny_file gDefault = { 0U, { 0U, 0U, 0U, 0U } };

#define GENERAL_EVENT_LOG_ENTRIES   100

typedef enum {
    APP_CFG_TINY,
    APP_CFG_LOG,
    APP_CFG_BIG,
    APP_NO_FILE

} app_file_id;

static const fs_config cFiles[] = {

    // name             type           first_block         nb_records              record_size                  status
    { APP_CFG_TINY,     'S',            0U,                     2U,             TINY_FILE_SIZE,         0U },
//    { FS_FILE_2, 'F', 0U, 2U, GENERAL_EVENT_LOG_ENTRIES*sizeof(cfg_test_sample_log), 0U },
};

#define NB_FILES (sizeof(cFiles) / sizeof(&cFiles[0]))


static void setUp(void)
{

}

static void tearDown(void)
{

}

static void fs_test_open(void)
{
    fs_handle handle;

    fs_initialize(&cFiles[0], NB_FILES);

    // Test unknown file
    int ret = fs_open(&handle, APP_NO_FILE);
    TEST_ASSERT_EQUAL_INT(FS_FILE_NOT_FOUND, ret);

    ret = fs_open(&handle, APP_CFG_TINY);
    TEST_ASSERT_EQUAL_INT(FS_OK, ret);

    cfg_test_tiny_file file;

    ret = fs_read(&handle, &file, TINY_FILE_SIZE);

}

TestRef FileSystem_test(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture("fs_test_open",fs_test_open),
    };
    EMB_UNIT_TESTCALLER(FileSystemTest,"FileSystemTest", setUp, tearDown,fixtures);

    return (TestRef)&FileSystemTest;
}

