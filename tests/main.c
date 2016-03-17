
#include <stdlib.h>
#include <string.h>

// Cosem stack
#include "csm_array.h"
#include "csm_ber.h"
#include "csm_channel.h"
#include "csm_config.h"

// Meter environment
#include "unity_fixture.h"
#include "tcp_server.h"
#include "csm_database.h"
#include "os.h"
#include "bitfield.h"

/*
static void RunAllTests(void)
{
  RUN_TEST_GROUP(CosemArray);
}
*/

static const uint8_t association[] = {  0x60U,  0x36U, 0xA1U,   0x09U,   0x06U,   0x07U,  0x60U,  0x85U,
                                        0x74U,   0x05U,   0x08U,   0x01U,   0x01U, 0x8AU,   0x02U,   0x07U,
                                        0x80U, 0x8BU,   0x07U,  0x60U,  0x85U,  0x74U,   0x05U,   0x08U,
                                         0x02U,   0x01U, 0xACU, 0x0AU,  0x80U,   0x08U,  0x41U,  0x42U,
                                        0x43U,  0x44U,  0x45U,  0x46U,  0x47U,  0x48U, 0xBEU,  0x10U,
                                         0x04U, 0x0EU,   0x01U,   0x00U,   0x00U,   0x00U,   0x06U, 0x5FU,
                                       0x1FU,   0x04U,   0x00U,   0x00U,  0x30U, 0x1DU, 0xFFU, 0xFFU
 };

const csm_asso_config assos_config[] =
{
    // Client public association
    { {16U, 1U},
      CSM_CBLOCK_GET | CSM_CBLOCK_BLOCK_TRANSFER_WITH_GET_OR_READ,
      0U, // No auto-connected
      CSM_AUTH_LOWEST_LEVEL,             // No Authentication
      { 0x30U, 0x30U, 0x30U, 0x30U, 0x30U, 0x30U, 0x30U, 0x30U }, // Password.
    },

    // Client management association
    { {1U, 1U},
      CSM_CBLOCK_GET | CSM_CBLOCK_SET |CSM_CBLOCK_BLOCK_TRANSFER_WITH_GET_OR_READ | CSM_CBLOCK_SELECTIVE_ACCESS,
      0U, // No auto-connected
      CSM_AUTH_LOW_LEVEL, // Low Level Authentication
      { 0x30U, 0x30U, 0x30U, 0x30U, 0x30U, 0x30U, 0x30U, 0x31U }, // Password. 00000001
    }
};

#define NUMBER_OF_ASSOS (sizeof(assos_config) / sizeof(csm_asso_config))

csm_asso_state assos[NUMBER_OF_ASSOS];
csm_channel channels[2];

#define NUMBER_OF_CHANNELS (sizeof(channels) / sizeof(csm_channel))


static const uint16_t COSEM_WRAPPER_VERSION = 0x0001U;
#define COSEM_WRAPPER_SIZE 8U
#define BUF_SIZE (COSEM_PDU_SIZE + COSEM_WRAPPER_SIZE)


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
        version = GET_BE16(&buffer[0]);
        channels[channel].request.llc.ssap = GET_BE16(&buffer[2]);
        channels[channel].request.llc.dsap = GET_BE16(&buffer[4]);
        apdu_size = GET_BE16(&buffer[6]);

        // Sanity check of the packet
        if ((size == (apdu_size + COSEM_WRAPPER_SIZE)) &&(version == COSEM_WRAPPER_VERSION))
        {
            uint32_t i = 0U;
            // Seems to be a valid Cosem packet!
            // We have to find the association used by this request
            // Find the valid association.
            for (i = 0U; i < NUMBER_OF_ASSOS; i++)
            {
                if ((channels[channel].request.llc.ssap == assos_config[i].llc.ssap) &&
                    (channels[channel].request.llc.dsap == assos_config[i].llc.dsap))
                {
                    break;
                }
            }

            if (i < NUMBER_OF_ASSOS)
            {
                // Association found, use this one
                // Link the state with the configuration structure
                assos[i].config = &assos_config[i];
                channels[channel].asso = &assos[i];

                // Then decode the packet, the reply, if any is located in the buffer
                // The reply is valid if the return code is > 0
                csm_array_init(&packet, (uint8_t *)&buffer[COSEM_WRAPPER_SIZE], BUF_SIZE, apdu_size);
                ret = csm_channel_execute(&channels[channel].request, &assos[i], &packet);

                if (ret > 0)
                {
                    // Swap SSAP and DSAP
                    SET_BE16(&buffer[2], channels[channel].request.llc.dsap);
                    SET_BE16(&buffer[4], channels[channel].request.llc.ssap);

                    // Update Cosem Wrapper length
                    SET_BE16(&buffer[6], (uint16_t) ret);

                    // Add wrapper size to the data packet size
                    ret += COSEM_WRAPPER_SIZE;
                }
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
            csm_channel_disconnect(&channels[channel]);
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
        // search for a valid free channel
        for (uint32_t i = 0U; i < NUMBER_OF_CHANNELS; i++)
        {
            if (channels[i].id == INVALID_CHANNEL_ID)
            {
                ret = i + 1U; // transform into channel id
                CSM_LOG("[LLC] Grant connection to channel %d", channel);
                break;
            }
        }

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
    return ret;
}


// Application & stack initialization
void csm_init()
{
    // 1. DLMS/Cosem stack initialization
    csm_services_init(csm_db_access_func);

    for (uint32_t i = 0U; i < NUMBER_OF_ASSOS; i++)
    {
        csm_asso_init(&assos[i]);
    }

    for (uint32_t i = 0U; i < NUMBER_OF_CHANNELS; i++)
    {
        csm_channel_init(&channels[i]);
    }

}
/*
LN referencing with no ciphering,
lowest level security;
60 1D A1 09 06 07 60 85 74 05 08 01 01 BE 10 04
0E 01 00 00 00 06 5F 1F 04 00 00 7E 1F 04 B0
LN referencing with no ciphering,
low level security;
60 36 A1 09 06 07 60 85 74 05 08 01 01 8A 02 07
80 8B 07 60 85 74 05 08 02 01 AC 0A 80 08 31 32
33 34 35 36 37 38 BE 10 04 0E 01 00 00 00 06 5F
1F 04 00 00 7E 1F 04 B0
LN referencing with no ciphering,
high level security;
60 36 A1 09 06 07 60 85 74 05 08 01 01 8A 02 07
80 8B 07 60 85 74 05 08 02 05 AC 0A 80 08 4B 35
36 69 56 61 67 59 BE 10 04 0E 01 00 00 00 06 5F
1F 04 00 00 7E 1F 04 B0
*/

int main(int argc, const char * argv[])
{

  //return UnityMain(argc, argv, RunAllTests);

    (void) argc;
    (void) argv;
/*
    uint32_t size = sizeof(association);

    // Intermediate buffer
    uint8_t buffer[size];
    memcpy(buffer, association, size);

    csm_array array;
    csm_array_alloc(&array, &buffer[0], size);

    //csm_array_dump(&array2);
*/

    csm_init();


    char buffer[BUF_SIZE];

    return tcp_server_init(tcp_data_handler, tcp_conn_handler, buffer, BUF_SIZE);
}

