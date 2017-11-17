
#include <stdlib.h>
#include <string.h>
#include <time.h> // to initialize the seed

// Cosem stack
#include "csm_array.h"
#include "csm_ber.h"
#include "csm_channel.h"
#include "csm_config.h"

// Meter environment
#include "os_util.h"
#include "bitfield.h"
#include "server_config.h"
#include "clock.h"

// Unit test framework
#define CATCH_CONFIG_RUNNER
#include "catch.hpp"

//-----------------------------------------------------------------------------
// Prototypes
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Implementation
//-----------------------------------------------------------------------------
int main( int argc, char* argv[] )
{
    Catch::Session session; // There must be exactly one instance

    // writing to session.configData() here sets defaults
    // this is the preferred way to set them

    int returnCode = session.applyCommandLine( argc, argv );
    if( returnCode != 0 ) // Indicates a command line error
    return returnCode;

    // writing to session.configData() or session.Config() here
    // overrides command line args
    // only do this if you know you need to

    int numFailed = session.run();
    // Note that on unices only the lower 8 bits are usually used, clamping
    // the return value to 255 prevents false negative when some multiple
    // of 256 tests has failed
    return ( numFailed < 0xff ? numFailed : 0xff );
}

