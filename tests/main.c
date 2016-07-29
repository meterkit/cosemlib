
#include <stdlib.h>
#include <string.h>
#include <time.h> // to initialize the seed

// Cosem stack
#include "csm_array.h"
#include "csm_ber.h"
#include "csm_channel.h"
#include "csm_config.h"

// Meter environment
//#include "unity_fixture.h"
#include "tcp_server.h"
#include "app_database.h"
#include "os.h"
#include "bitfield.h"
#include "server_config.h"

/*
static void RunAllTests(void)
{
    RUN_TEST_GROUP(CosemArray);
    RUN_TEST_GROUP(Aes128Gcm);
    RUN_TEST_GROUP(FileSystem);
}


int main(int argc, const char * argv[])
{
    (void) argc;
    (void) argv;

    printf("Starting DLMS/Cosem unit tests\r\nCosem library version: %s\r\n\r\n", CSM_DEF_LIB_VERSION);
    return UnityMain(argc, argv, RunAllTests);
}
*/

#include <embUnit/embUnit.h>
#include <Outputter.h>
#include <TextOutputter.h>
#include <CompilerOutputter.h>
#include <XMLOutputter.h>
#include <TextUIRunner.h>

//-----------------------------------------------------------------------------
// Prototypes
//-----------------------------------------------------------------------------

TestRef FileSystem_test(void);


//-----------------------------------------------------------------------------
// Implementation
//-----------------------------------------------------------------------------

int main (int argc, const char* argv[])
{
    (void) argc;
    (void) argv;
    // FIXME: Change the outputter according to command line arguments

    //TextUIRunner_setOutputter(TextOutputter_outputter());
    TextUIRunner_setOutputter(XMLOutputter_outputter());

    TextUIRunner_start();
        TextUIRunner_runTest(FileSystem_test());
    TextUIRunner_end();

    return 0;
}

