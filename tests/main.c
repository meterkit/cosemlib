
#include <stdlib.h>
#include <string.h>

#include "unity_fixture.h"
#include "csm_array.h"
#include "csm_ber.h"
#include "csm_channel.h"
#include "tcp_server.h"

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

enum phy_channels
{
    PHY_REMOTE_TCP1 = 0x01U,
    PHY_REMOTE_TCP2 = 0x02U,
};

const csm_asso_config assos_config[] =
{
    // Client public association
    { {16U, 1U}, PHY_REMOTE_TCP1 | PHY_REMOTE_TCP2 ,
      CSM_GET | CSM_BLOCK_TRANSFER_WITH_GET_OR_READ,
      0U, // No auto-connected
      CSM_LOWEST_AUTHENTICATION,             // No Authentication
      { 0x30U, 0x30U, 0x30U, 0x30U, 0x30U, 0x30U, 0x30U, 0x30U }, // Password.
    },

    // Client manual meter reader association
    { {1U, 1U}, PHY_REMOTE_TCP1 ,
      CSM_GET | CSM_BLOCK_TRANSFER_WITH_GET_OR_READ | CSM_SELECTIVE_ACCESS,
      0U, // No auto-connected
      CSM_LOW_AUTHENTICATION, // Low Level Authentication
      { 0x30U, 0x30U, 0x30U, 0x30U, 0x30U, 0x30U, 0x30U, 0x31U }, // Password. 00000001
    }
};

#define NUMBER_OF_ASSOS (sizeof(assos_config) / sizeof(csm_asso_config))

csm_asso_state assos[NUMBER_OF_ASSOS];
csm_channel channels[2];

#define NUMBER_OF_CHANNELS (sizeof(channels) / sizeof(csm_channel))

uint16_t get_uint16(char *buff)
{
    uint16_t val = ((uint16_t)buff[0]) << 8U;
    val += buff[1] & 0xFFU;
    return val;
}

uint8_t is_bit_set(uint8_t value, uint8_t bit)
{
    return ((value & (1U<<bit)) == 0U) ? 0U : 1U;
}

static const uint16_t COSEM_WRAPPER_VERSION = 0x0001U;
static const uint16_t COSEM_WRAPPER_SIZE = 8U;
/**
 * @brief datalink_layer
 * This link layer manages the data between the transport (TCP/IP) and the Cosem stack
 * The function is called by the TCP/IP server upon reception of a neww packet.
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
int datalink_layer(uint8_t channel, char *buffer, size_t size)
{
    int ret = -1;
    uint16_t version;
    uint16_t apdu_size;
    csm_llc llc;
    csm_array packet;

    // The TCP/IP Cosem packet is sent with a header. See GreenBook 8 7.3.3.2 The wrapper protocol data unit (WPDU)

    // Version, 2 bytes
    // Source wPort, 2 bytes
    // Destination wPort: 2 bytes
    // Length: 2 bytes
    CSM_LOG("[LLC] TCP Packet received");

    if ((size > COSEM_WRAPPER_SIZE) && (channel < NUMBER_OF_CHANNELS))
    {
        version = get_uint16(&buffer[0]);
        llc.ssap = get_uint16(&buffer[2]);
        llc.dsap = get_uint16(&buffer[4]);
        apdu_size = get_uint16(&buffer[6]);

        // Sanity check of the packet
        if ((size == (apdu_size + COSEM_WRAPPER_SIZE)) &&(version == COSEM_WRAPPER_VERSION))
        {
            uint32_t i = 0U;
            // Seems to be a valid Cosem packet!
            // We have to find the association used by this request
            // Find the valid association.
            for (i = 0U; i < NUMBER_OF_ASSOS; i++)
            {
                if ((llc.ssap == assos_config[i].llc.ssap) &&
                    (llc.dsap == assos_config[i].llc.dsap) &&
                    is_bit_set(assos_config[i].channels, channel))
                {
                    break;
                }
            }

            if (i < NUMBER_OF_ASSOS)
            {
                // Association found, use this one
                // Link the state with the configuration structure
                assos[i].config = &assos_config[i];
                // Then decode the packet, the reply, if any is located in the buffer
                // The reply is valid if the return code is > 0
                csm_array_alloc(&packet, (uint8_t *)&buffer[COSEM_WRAPPER_SIZE], apdu_size);
                ret = csm_channel_execute(&channels[channel], &assos[i], &llc, &packet);
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

// Main stack initialization
void csm_init()
{
    for (uint32_t i = 0U; i < NUMBER_OF_ASSOS; i++)
    {
        csm_asso_init(&assos[i]);
    }
}


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

    return tcp_server_init(datalink_layer);
}

