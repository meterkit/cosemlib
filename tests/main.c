
#include <stdlib.h>
#include <string.h>
#include <time.h> // to initialize the seed

// Cosem stack
#include "csm_array.h"
#include "csm_ber.h"
#include "csm_channel.h"
#include "csm_config.h"

// Meter environment
#include "unity_fixture.h"
#include "tcp_server.h"
#include "app_database.h"
#include "os.h"
#include "bitfield.h"
#include "server_config.h"

#ifdef TESTS
static void RunAllTests(void)
{
    RUN_TEST_GROUP(CosemArray);
    RUN_TEST_GROUP(Aes128Gcm);
}

#else

const csm_asso_config assos_config[] =
{
    // Client public association
    { {16U, 1U},
      CSM_CBLOCK_GET | CSM_CBLOCK_BLOCK_TRANSFER_WITH_GET_OR_READ,
      0U, // No auto-connected
    },

    // Client management association
    { {1U, 1U},
      CSM_CBLOCK_GET | CSM_CBLOCK_ACTION | CSM_CBLOCK_SET |CSM_CBLOCK_BLOCK_TRANSFER_WITH_GET_OR_READ | CSM_CBLOCK_SELECTIVE_ACCESS,
      0U, // No auto-connected
    }
};

#define NUMBER_OF_ASSOS (sizeof(assos_config) / sizeof(csm_asso_config))

csm_asso_state assos[NUMBER_OF_ASSOS];
csm_channel channels[NUMBER_OF_CHANNELS];


// Buffer has the following format:
//   | SC + AK | wrapper | APDU
// With:
// SC+AK: security AAD header, room booked to optimize buffer management with cyphering
// wrapper: Cosem standard TCP wrapper (4 words)
// APDU: Cosem APDU

static const uint16_t COSEM_WRAPPER_VERSION = 0x0001U;
#define COSEM_WRAPPER_SIZE 8U
#define BUF_SIZE (CSM_DEF_PDU_SIZE + CSM_DEF_MAX_HLS_SIZE + COSEM_WRAPPER_SIZE)


#define BUF_WRAPPER_OFFSET  (CSM_DEF_MAX_HLS_SIZE)
#define BUF_APDU_OFFSET     (COSEM_WRAPPER_SIZE + CSM_DEF_MAX_HLS_SIZE)


static char gBuffer[BUF_SIZE];

/**
 * @brief tcp_data_handler
 * This link layer manages the data between the transport (TCP/IP) and the Cosem stack
 * The function is called by the TCP/IP server upon reception of a new packet.
 * The passed buffer must be filled by the reply, if any, and return the according number
 * of bytes to transfer back to the sender.
 *
 * The data link layer is application specific - but rather simple - and must be implemented
 * in the application side. Thus, the DLMS/Cosem stack remains agnostic on the transport layer.
 *
 * @param buffer
 * @param size
 * @return > 0 the number of bytes to reply back to the sender
 */
int tcp_data_handler(uint8_t channel, uint8_t *buffer, size_t size)
{
    int ret = -1;
    uint16_t version;
    uint16_t apdu_size;
    csm_array packet;

    // The TCP/IP Cosem packet is sent with a header. See GreenBook 8 7.3.3.2 The wrapper protocol data unit (WPDU)

    // Version, 2 bytes
    // Source wPort, 2 bytes
    // Destination wPort: 2 bytes
    // Length: 2 bytes
    CSM_LOG("[LLC] TCP Packet received");

    if ((size > COSEM_WRAPPER_SIZE) && (channel < NUMBER_OF_CHANNELS))
    {
        version = GET_BE16(&buffer[0U]);
        channels[channel].request.llc.ssap = GET_BE16(&buffer[2]);
        channels[channel].request.llc.dsap = GET_BE16(&buffer[4]);
        apdu_size = GET_BE16(&buffer[6]);

        // Sanity check of the packet
        if ((size == (apdu_size + COSEM_WRAPPER_SIZE)) &&(version == COSEM_WRAPPER_VERSION))
        {
            // Then decode the packet, the reply, if any is located in the buffer
            // The reply is valid if the return code is > 0
            csm_array_init(&packet, (uint8_t *)&gBuffer[0], BUF_SIZE, apdu_size, BUF_APDU_OFFSET);
            ret = csm_channel_execute(channel, &packet);

            if (ret > 0)
            {
                // Set Version
                SET_BE16(&buffer[0], version);

                // Swap SSAP and DSAP
                SET_BE16(&buffer[2], channels[channel].request.llc.dsap);
                SET_BE16(&buffer[4], channels[channel].request.llc.ssap);

                // Update Cosem Wrapper length
                SET_BE16(&buffer[6], (uint16_t) ret);

                // Add wrapper size to the data packet size
                ret += COSEM_WRAPPER_SIZE;
            }
        }
        else
        {
            CSM_ERR("[LLC] Bad Packet received");
        }
    }
    else
    {
        CSM_ERR("[LLC] Bad Packet received");
    }

    return ret;
}

uint8_t tcp_conn_handler(uint8_t channel, enum conn_event event)
{
    uint8_t ret = FALSE;
    switch(event)
    {
    case CONN_DISCONNECTED:
    {
        if (channel > INVALID_CHANNEL_ID)
        {
            channel--; // transform id into index
            csm_channel_disconnect(channel);
            ret = TRUE;
            CSM_ERR("[LLC] Channel %d disconnected", channel);
        }
        else
        {
            CSM_ERR("[LLC] Channel id invalid");
        }
        break;
    }

    case CONN_NEW:
    {
        ret = csm_channel_new();
        if (!ret)
        {
            CSM_ERR("[LLC] Cannot find free channel slot");
        }

        break;
    }

    default:
        CSM_ERR("[LLC] Received spurious event");
        break;
    }
    return ret; // Returns error or the channel id
}


// Application & stack initialization
void csm_init()
{
    srand(time(NULL)); // seed init

    // Debug: fill buffer with pattern
    memset(&gBuffer[0], 0xAA, BUF_SIZE);

    // DLMS/Cosem stack initialization
    csm_services_init(csm_db_access_func);
    csm_channel_init(&channels[0], NUMBER_OF_CHANNELS, &assos[0], &assos_config[0], NUMBER_OF_ASSOS);
}

#endif

int main(int argc, const char * argv[])
{
    (void) argc;
    (void) argv;

#ifdef TESTS
    printf("Starting DLMS/Cosem unit tests\r\nCosem library version: %s\r\n\r\n", CSM_DEF_LIB_VERSION);
    return UnityMain(argc, argv, RunAllTests);
#else

    csm_init();
    printf("Starting DLMS/Cosem example\r\nCosem library version: %s\r\n\r\n", CSM_DEF_LIB_VERSION);

    return tcp_server_init(tcp_data_handler, tcp_conn_handler, &gBuffer[BUF_WRAPPER_OFFSET], BUF_SIZE, TCP_PORT);
#endif
}

