
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
#include "serial.h"
#include "util.h"
#include "bitfield.h"
#include "client_config.h"


char *cnx_hdlc = "7EA0210002FEFFF193E7C8818012050180060180070400000001080400000007655E7E";



// Application & stack initialization
void csm_init()
{
    srand(time(NULL)); // seed init

    // DLMS/Cosem stack initialization
//    csm_services_init(csm_db_access_func);
 //   csm_channel_init(&channels[0], NUMBER_OF_CHANNELS, &assos[0], &assos_config[0], NUMBER_OF_ASSOS);
}

uint8_t buf_in[4*1024] = "Coucou !";
uint8_t buf_out[4*1024];

static const uint8_t snrm[] = {0x7E, 0xA0, 0x21, 0x00, 0x02, 0x00, 0x23, 0x03, 0x93, 0x9A, 0x74, 0x81, 0x80, 0x12,
                               0x05, 0x01, 0x80, 0x06, 0x01, 0x80, 0x07, 0x04, 0x00, 0x00, 0x00, 0x01, 0x08, 0x04, 0x00, 0x00, 0x00, 0x07, 0x65, 0x5E, 0x7E };

#define BUF_IN_SIZE sizeof(buf_in)
#define BUF_OUT_SIZE sizeof(buf_out)

int main(int argc, const char * argv[])
{
    (void) argc;
    (void) argv;

    csm_init();
    printf("Starting DLMS/Cosem client example\r\nCosem library version: %s\r\n\r\n", CSM_DEF_LIB_VERSION);

    int ser_handle = serial_open("COM4");
    serial_setup(ser_handle, 19200);

    //hex2bin(cnx_hdlc, buf_in, sizeof(cnx_hdlc));

    serial_write(ser_handle, (char*)snrm, sizeof(snrm));//sizeof(cnx_hdlc)/2);
    int ret = serial_read(ser_handle, (char*)buf_out, BUF_OUT_SIZE);

    print_hex(buf_out, ret);

    return 0;
}

