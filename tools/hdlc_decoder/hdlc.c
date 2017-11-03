#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "hdlc.h"


// Input file must contains only ANSI hexadecimal characters without spaces
int main(int argc, char** argv)
{
    if (argc == 2)
    {
        char *buffer = NULL;
        size_t sz = 0U;
        FILE *f = fopen(argv[1], "r");
        if (f != NULL)
        {
            fseek(f, 0L, SEEK_END);
            sz = ftell(f);
            rewind(f);
            printf("File size: %d\r\n", sz);

            buffer = malloc(sz);
            if (buffer != NULL)
            {
                fread(buffer, 1, sz, f);
                // now close the file as we have finish with it
                fclose(f);
            }
            else
            {
                printf("Cannot allocate memory!\r\n");
            }
        }
        else
        {
            printf("Not a file, reading input string\r\n");
            sz = strlen(argv[1]);
            printf("String size: %d\r\n", sz);
            buffer = malloc(sz);
            if (buffer != NULL)
            {
                memcpy(buffer, argv[1], sz);
            }
            else
            {
                printf("Cannot allocate memory!\r\n");
            }
        }

        // Simple test: we must have a even number of bytes
        if (!(sz%2))
        {
            printf("Input string: ");
            for(int i = 0; i<sz; i++) printf("%c", buffer[i]);
            printf("\r\n");

            // Now, transform the hexadecimal string into an array of integers
            size_t packet_size = sz/2U;
            uint8_t *packet = malloc(packet_size);

            if (packet != NULL)
            {
                int ret;

                hex2bin(buffer, packet, sz);
                //print_hex(packet, packet_size);

                hdlc_t hdlc;
                hdlc_init(&hdlc);

                ret = hdlc_decode(&hdlc, packet, packet_size);
                hdlc_print_result(&hdlc, ret);

                free(packet);
            }
            else
            {
                printf("Cannot allocate memory!\r\n");
            }
        }
        else
        {
            printf("String size must have a event number of characters!\r\n");
        }

        if (buffer != NULL)
        {
            free(buffer);
        }
    }
    else
    {
        printf("HDLC packet decoder, from input file or a HEX string as parameter.\r\n");
        printf("Usage: %s [inputfile.txt] [7E...7E]\r\n", argv[0]);
    }

    return 0;
}
