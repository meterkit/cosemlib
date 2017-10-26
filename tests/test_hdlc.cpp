
#include "os_util.h"
#include "hdlc.h"

#include "catch.hpp"
#include <iostream>

// The following packet contains three HDLC frames.
// We are using it to test the streaming capability of the decoder
static const char multi_frames[] = "7EA88C0300020023421CB9E6E700C401C10001010205020209060000600100FF0A08736E303030303030020509060100628601FF1224D502020F001604020211001101090C07E10A19030D1813FF8000FF01010206020309060101020800FF060000000002020F03161E020309060101010800FF060000000002020F03161E020309060101050800FF060096CC7E7EA88C0300020023442ADC00000002020F001620020309060101060800FF060000000002020F001620020309060101070800FF060000000002020F001620020309060101080800FF060000000002020F00162001010201020509060101010801FF060000000002020F00161E0000010102020906000062853DFF0105020312000002020F00161B090C07E1EED87E7EA07A030002002356990D0A19FF0D1813FF8000FF020312000002020F00161B090C07E10A19FF0D1503FF8000FF020312000002020F00161B090C07C80106FF161E00FF8000FF020312000002020F00161B090C07C80106FF160F00FF8000FF020312000002020F00161B090C07C80106FF160000FF8000FF05B47E";


// Quick simple tests to checks that the clock is working in nominal way
// Not full test
void StreamingDecoder()
{
    // transform the hexadecimal string into an array of integers
    int sz = sizeof(multi_frames);

    size_t packet_size = sz/2U;
    uint8_t *packet = (uint8_t *) malloc(packet_size);

    if (packet != NULL)
    {
        int ret;

        hex2bin(multi_frames, (char *)packet, sz);
        //print_hex(packet, packet_size);

        hdlc_t hdlc;
        hdlc_init(&hdlc);

        ret = hdlc_decode(&hdlc, packet, packet_size);

        REQUIRE(HDLC_OK == ret);

        debug_print("HDLC: frame_size=%d, data_size=%d, data_index=%d\r\n", hdlc.frame_size, hdlc.data_size, hdlc.data_index);

        debug_puts("HDLC data::\r\n");
        print_hex((const char*)&packet[hdlc.data_index], hdlc.data_size);

        free(packet);
    }
    else
    {
       printf("Cannot allocate memory!\r\n");
    }

}


TEST_CASE( "HDLC", "[hdlc_test]" )
{
    StreamingDecoder();

}



