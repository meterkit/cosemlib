#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#ifndef BIT
#define BIT(x) (1U << (x))
#endif

/*
7EA00A00020023F193232E7E


7E A00A : frame type=3, size=10
   0002 0023 // dest address (server)
      ^    ^
      |  	|___ physical address (0x23 / 2 = 17)
	  |_________ logical device (2 / 2 = 1)
   F1 : client address (0xF1 / 2 = 120)
    
   93 : control field (here: SNRM)
         100 1    0011
		     ^
			 P/F
   
   
   232E : FCS (no info field)
7E

*/

static const uint16_t fcstab[256] = { 
     0x0000, 0x1189, 0x2312, 0x329b, 0x4624, 0x57ad, 0x6536, 0x74bf, 
     0x8c48, 0x9dc1, 0xaf5a, 0xbed3, 0xca6c, 0xdbe5, 0xe97e, 0xf8f7, 
     0x1081, 0x0108, 0x3393, 0x221a, 0x56a5, 0x472c, 0x75b7, 0x643e, 
     0x9cc9, 0x8d40, 0xbfdb, 0xae52, 0xdaed, 0xcb64, 0xf9ff, 0xe876, 
     0x2102, 0x308b, 0x0210, 0x1399, 0x6726, 0x76af, 0x4434, 0x55bd, 
     0xad4a, 0xbcc3, 0x8e58, 0x9fd1, 0xeb6e, 0xfae7, 0xc87c, 0xd9f5, 
     0x3183, 0x200a, 0x1291, 0x0318, 0x77a7, 0x662e, 0x54b5, 0x453c, 
     0xbdcb, 0xac42, 0x9ed9, 0x8f50, 0xfbef, 0xea66, 0xd8fd, 0xc974, 
     0x4204, 0x538d, 0x6116, 0x709f, 0x0420, 0x15a9, 0x2732, 0x36bb, 
     0xce4c, 0xdfc5, 0xed5e, 0xfcd7, 0x8868, 0x99e1, 0xab7a, 0xbaf3, 
     0x5285, 0x430c, 0x7197, 0x601e, 0x14a1, 0x0528, 0x37b3, 0x263a, 
     0xdecd, 0xcf44, 0xfddf, 0xec56, 0x98e9, 0x8960, 0xbbfb, 0xaa72, 
	 0x6306, 0x728f, 0x4014, 0x519d, 0x2522, 0x34ab, 0x0630, 0x17b9, 
     0xef4e, 0xfec7, 0xcc5c, 0xddd5, 0xa96a, 0xb8e3, 0x8a78, 0x9bf1, 
     0x7387, 0x620e, 0x5095, 0x411c, 0x35a3, 0x242a, 0x16b1, 0x0738, 
     0xffcf, 0xee46, 0xdcdd, 0xcd54, 0xb9eb, 0xa862, 0x9af9, 0x8b70, 
     0x8408, 0x9581, 0xa71a, 0xb693, 0xc22c, 0xd3a5, 0xe13e, 0xf0b7, 
     0x0840, 0x19c9, 0x2b52, 0x3adb, 0x4e64, 0x5fed, 0x6d76, 0x7cff, 
     0x9489, 0x8500, 0xb79b, 0xa612, 0xd2ad, 0xc324, 0xf1bf, 0xe036, 
     0x18c1, 0x0948, 0x3bd3, 0x2a5a, 0x5ee5, 0x4f6c, 0x7df7, 0x6c7e, 
     0xa50a, 0xb483, 0x8618, 0x9791, 0xe32e, 0xf2a7, 0xc03c, 0xd1b5, 
     0x2942, 0x38cb, 0x0a50, 0x1bd9, 0x6f66, 0x7eef, 0x4c74, 0x5dfd, 
     0xb58b, 0xa402, 0x9699, 0x8710, 0xf3af, 0xe226, 0xd0bd, 0xc134, 
     0x39c3, 0x284a, 0x1ad1, 0x0b58, 0x7fe7, 0x6e6e, 0x5cf5, 0x4d7c, 
     0xc60c, 0xd785, 0xe51e, 0xf497, 0x8028, 0x91a1, 0xa33a, 0xb2b3, 
     0x4a44, 0x5bcd, 0x6956, 0x78df, 0x0c60, 0x1de9, 0x2f72, 0x3efb, 
     0xd68d, 0xc704, 0xf59f, 0xe416, 0x90a9, 0x8120, 0xb3bb, 0xa232, 
     0x5ac5, 0x4b4c, 0x79d7, 0x685e, 0x1ce1, 0x0d68, 0x3ff3, 0x2e7a, 
     0xe70e, 0xf687, 0xc41c, 0xd595, 0xa12a, 0xb0a3, 0x8238, 0x93b1, 
     0x6b46, 0x7acf, 0x4854, 0x59dd, 0x2d62, 0x3ceb, 0x0e70, 0x1ff9, 
     0xf78f, 0xe606, 0xd49d, 0xc514, 0xb1ab, 0xa022, 0x92b9, 0x8330, 
     0x7bc7, 0x6a4e, 0x58d5, 0x495c, 0x3de3, 0x2c6a, 0x1ef1, 0x0f78 
}; 

#define PPPINITFCS16    0xffff  /* Initial FCS value */ 
/* 
* Calculate a new fcs given the current fcs and the new data. 
*/ 
uint16_t pppfcs16(uint16_t fcs, const uint8_t* cp, uint32_t len) 
{ 
	while (len--)
	{
		fcs = (fcs >> 8) ^ fcstab[(fcs ^ *cp++) & 0xff]; 
	}
	return (fcs); 
} 


static uint8_t is_bit_set(uint8_t value, uint8_t bit)
{
    return ((value & BIT(bit)) == 0U) ? 0U : 1U;
}

void print_hex(const uint8_t *buf, size_t size)
{
	size_t i = 0U;
	static const char binHex[] = "0123456789ABCDEF";

	for (i = 0U; i < size; i++)
    {
        uint8_t byte = buf[i];
        printf("%C", binHex[(byte >> 4) & 0x0F]);
        printf("%c", binHex[byte & 0x0F]);
    }
}
   

int hex2bin(const char *in, uint8_t* out, size_t size)
{
    size_t i = 0;
	size_t j = 0;

    size_t final_len = size / 2U;

    for (i = 0, j = 0; j<final_len; i+=2, j++)
	{
        out[j] = (in[i] % 32 + 9) % 25 * 16 + (in[i+1] % 32 + 9) % 25;
	}
}

#define HDLC_OK			0
#define HDLC_ERR		-1
#define HDLC_ERR_7E		-2
#define HDLC_ERR_FORMAT	-3
#define HDLC_ERR_SIZE	-4 // bad packet size
#define HDLC_ERR_ADDR	-5 // address format invalid
#define HDLC_ERR_FCS	-6

typedef struct
{
	uint16_t len;
	uint16_t logical_device;
	uint16_t phy_address;
	uint8_t client_addr; // Client is always one byte length
	uint8_t addr_len; // server addressing scheme (1, 2 or 4 bytes)
	uint8_t segmentation;
	uint8_t rrr;
	uint8_t sss;
	uint8_t type;
	uint8_t poll_final;
} hdlc_t;

#define HDLC_FORMAT_TYPE	(0xA0)
#define HDLC_SEGMENTATION	(0x08)
#define HDLC_LEN_HI			(0x03)
#define HDLC_POLL_FINAL_BIT	(5)

static uint16_t hdlc_get_len(const uint8_t *buf)
{
	uint16_t len = (buf[0] & HDLC_LEN_HI) << 11;
	return (len + buf[1]);
}

/*
8.4.1.1  HDLC frame format type 3 
The MAC sublayer uses the HDLC frame format type 3 as defined in Annex H.4 of ISO/IEC 13239. It 
is shown on Figure 49: 
Flag  Frame format  Dest. address  Src. address  Control  HCS  Information  FCS  Flag 
*/

void hdlc_init(hdlc_t *hdlc)
{
	hdlc->logical_device = 0U;
	hdlc->phy_address = 0U;
	hdlc->client_addr = 0U;
	hdlc->addr_len = 0;
	hdlc->segmentation = 0U;
	hdlc->len = 0U;
	hdlc->rrr = 0U;
	hdlc->sss = 0U;
	hdlc->type = 0U;
	hdlc->poll_final = 0U;
}

/**

The client address shall always be expressed on one byte. 
The  server  address  –  to  enable  addressing  more  than  one  logical  device  within  a  single  physical 
device and to support the multi-drop configuration – may be divided into two parts:  
•  the  upper  HDLC  address  is  used  to  address  a  Logical  Device  (a  separately  addressable  entity 
within a physical device); 
•  the  lower  HDLC  address  is  used  to  address  a  Physical  Device  (a  physical  device  on  the  multi-
drop).  
The upper HDLC address shall always be present. The  lower HDLC  address may  be omitted if it is 
not required. 

*/

static const uint8_t cMAX_SIZE = 4U; // address size is 4 bytes max

uint8_t hdlc_decode_addr_size(const uint8_t *buf, uint16_t size)
{
	uint8_t addr_size = 0U;
	uint8_t i = 0U;
	
	// Compute limit of the loop
	uint8_t max = (size < cMAX_SIZE) ? size : cMAX_SIZE;
	
	do
	{
		addr_size += 1;
		i++;
	}
	while ((i < max) && ((buf[i] % 2U) == 0));
	return addr_size;
}

int hdlc_get_addr(const uint8_t *buf, uint8_t addr_size, uint8_t *upper, )
{
	int ret = HDLC_OK;
	switch(addr_size)
	{
	case 1:
		*addr = buff[0];
		break;
	case 2:
		return "(size:2, upper:" +(val(str, index) >> 1).toString(10) +
		" ,lower:" + (val(str, index+2) >> 1).toString(10) + ")";
		break;
	case 4:
		return "(size:4, upper:" +(((val(str, index) >> 1) << 7 ) +
		(val(str, index + 2) >> 1)).toString(10) +
		" ,lower:" + (((val(str, index + 4) >> 1) << 7) +
		(val(str, index+6) >> 1)).toString(10) + ")";
		break;
	 default:
	   ret = HDLC_ERR_ADDR;
	}
	
	return ret;
}


int hdlc_decode_dest_addr(hdlc_t *hdlc, const uint8_t *buf, uint16_t size)
{
	int ret = HDLC_OK;
	
	uint8_t addr_size = hdlc_decode_addr_size(buf, size);
	
	
	uint16_t ld = buf[0];
	uint16_t phy;
	
	if ((ld & 0x01U) == 0x00)
	{
		// Then maybe it is coded on two bytes
		phy = buf[1];
		
		if ((phy & 0x01U) == 0x00)
		{
			// No, it is a 4-bytes addressing scheme
			ld = ld + phy;
			
			// Decode phy: last two bytes
			// 3rd byte must have the bit 0 at 0
			// 4th byte must have the bit 0 at 1 (indicate the end of the address)
			phy = buf[2];
			if (((phy & 0x01U) == 0x00U) &&
			    ((buf[3] & 0x01U) == 0x01U))
			{
				phy = (phy << 8U) + buf[3];
				hdlc->addr_len = 4U;
			}
			else
			{
				ret = HDLC_ERR_ADDR;
			}
		}
		else
		{
			hdlc->addr_len = 2U;
		}
	}
	else
	{
		hdlc->addr_len = 1U;
	}
	
	hdlc->logical_device = ld >> 1U;
	hdlc->phy_address = phy >> 1U;
	
	return ret;
}

int hdlc_decode_client_addr(hdlc_t *hdlc, const uint8_t *buf)
{
	int ret = HDLC_ERR_ADDR;
	if ((buf[0] & 0x01U) == 0x01U)
	{
		hdlc->client = buf[0] >> 1U;
		ret = HDLC_OK;
	}
		
	return ret;
}

// Packet types
#define HDLC_PACKET_TYPE_BAD	(0)
#define HDLC_PACKET_TYPE_I		(1)
#define HDLC_PACKET_TYPE_RR		(2)
#define HDLC_PACKET_TYPE_RNR	(3)
#define HDLC_PACKET_TYPE_SNRM	(4)
#define HDLC_PACKET_TYPE_DISC	(5)
#define HDLC_PACKET_TYPE_UA		(6)
#define HDLC_PACKET_TYPE_DM		(7)
#define HDLC_PACKET_TYPE_FRMR	(8)
#define HDLC_PACKET_TYPE_UI		(9)

#define HDLC_CF_POLL	0
#define HDLC_CF_FINAL	1

const char* hdlc_packet_to_string(hdlc_t *hdlc)
{
	if (hdlc->type == HDLC_PACKET_TYPE_I)
	{
		return "Information";
	}
	else if (hdlc->type == HDLC_PACKET_TYPE_RR)
	{
		return "RR";
	}
	else if (hdlc->type == HDLC_PACKET_TYPE_RNR)
	{
		return "RNR";
	}
	else if (hdlc->type == HDLC_PACKET_TYPE_SNRM)
	{
		return "SNRM";
	}
	else if (hdlc->type == HDLC_PACKET_TYPE_DISC)
	{
		return "DISC";
	}
	else if (hdlc->type == HDLC_PACKET_TYPE_UA)
	{
		return "UA";
	}
	else if (hdlc->type == HDLC_PACKET_TYPE_DM)
	{
		return "DM";
	}
	else if (hdlc->type == HDLC_PACKET_TYPE_FRMR)
	{
		return "FRMR";
	}
	else if (hdlc->type == HDLC_PACKET_TYPE_UI)
	{
		return "UI";
	}
	else
	{
		return "Bad packet";
	}
}


typedef struct
{
	uint8_t type;	// HDLC packet type
	uint8_t mask;	// Mask to apply to detect the packet type
	uint8_t value;	// Expected value that must have after the mask has been applied
} hdlc_packet_t;

/**
 *
                       MSB             LSB 
  Command    Response   
	I  			I  		R R R P/F S S S 0 
	RR  		RR  	R R R P/F 0 0 0 1 
	RNR  		RNR  	R R R P/F 0 1 0 1 
	SNRM    			1 0 0  P  0 0 1 1 
	DISC    			0 1 0  P  0 0 1 1 
				UA 		0 1 1  F  0 0 1 1 
				DM  	0 0 0  F  1 1 1 1 
				FRMR  	1 0 0  F  0 1 1 1 
	UI  		UI  	0 0 0 P/F 0 0 1 1 
*/

static const hdlc_packet_t cPACKET_TYPES[] = {
	{ HDLC_PACKET_TYPE_I, 		0x01U, 0x00U },
	{ HDLC_PACKET_TYPE_RR, 		0x0FU, 0x01U },
	{ HDLC_PACKET_TYPE_RNR, 	0x0FU, 0x05U },
	{ HDLC_PACKET_TYPE_SNRM, 	0xEFU, 0x83U },
	{ HDLC_PACKET_TYPE_DISC, 	0xEFU, 0x43U },
	{ HDLC_PACKET_TYPE_UA, 		0xEFU, 0x63U },
	{ HDLC_PACKET_TYPE_DM, 		0xEFU, 0x0FU },
	{ HDLC_PACKET_TYPE_FRMR, 	0xEFU, 0x87U },
	{ HDLC_PACKET_TYPE_UI, 		0xEFU, 0x03U }
};

static const uint32_t cNUMBER_OF_PACKETS = sizeof(cPACKET_TYPES)/sizeof(cPACKET_TYPES[0]);

int hdlc_decode_control_field(hdlc_t *hdlc, const uint8_t cf)
{
	int ret = HDLC_ERR;
	
	for (uint32_t i = 0U; i < cNUMBER_OF_PACKETS; i++)
	{
		if ((cf & cPACKET_TYPES[i].mask) == cPACKET_TYPES[i].value)
		{
			hdlc->type = cPACKET_TYPES[i].type;
			ret = HDLC_OK;
			break;
		}
	}
	
	hdlc->poll_final = is_bit_set(cf, HDLC_POLL_FINAL_BIT);
	// always extract rrr and sss even if it is not the right packet
	hdlc->rrr = (cf >> 5) & 0x07U;
	hdlc->sss = (cf >> 1) & 0x07U;
	
	return ret;
}

int hdlc_decode_info_field(hdlc_t *hdlc, const uint8_t *buf)
{
	int ret = HDLC_OK;
	
	return ret;
}

int hdlc_check_fcs(const uint8_t* buf, uint16_t size)
{
	uint16_t fcs = pppfcs16(PPPINITFCS16, &buf[1], size-4);
	
	// check last two bytes before the last 7E
	uint16_t expected = buf[size-2];
	expected = (expected << 8) + buf[size-3];
	
	fcs ^= 0xffff;
	
	printf("FCS calculated: 0x%.4X\r\n", fcs);
	printf("FCS expected: 0x%.4X\r\n", expected);
	
	return (expected == fcs) ? HDLC_OK : HDLC_ERR_FCS;
}


int hdlc_decode(hdlc_t *hdlc, const uint8_t *buf, uint16_t size)
{
	int ret = HDLC_ERR;

	// test packet structure
	if ((buf[0] == 0x7E) && (buf[size-1] == 0x7E))
	{
		// Test FCS, always present
		ret = hdlc_check_fcs(buf, size);
		
		if (!ret)
		{
			// next byte is the frame format
			uint8_t format = buf[1] & HDLC_FORMAT_TYPE;
			
			if (format == HDLC_FORMAT_TYPE)
			{
				hdlc->segmentation = is_bit_set(format, HDLC_SEGMENTATION);
				hdlc->len = hdlc_get_len(&buf[1]);
				
				// Sanity check: The value of the frame length subfield is the count of octets in the frame excluding the opening and 
				// closing frame flag sequences. 
				if ((hdlc->len + 2U) == size)
				{
					// FIXME: Sanity check: test a minimal size
					// 7E + frame format + dest + src +         + FCS + 7E
					//  1        2           1      1              2     1
					 
					const uint8_t* ptr = &buf[3];
					// Destination address decoder
					ret = hdlc_decode_dest_addr(hdlc, ptr);
					if (!ret)
					{
						ptr += hdlc->addr_len;
						ret = hdlc_decode_client_addr(hdlc, ptr);
						ptr++; // client address length is always 1 byte
						
						if (!ret)
						{
							// now decode the control field
							ret = hdlc_decode_control_field(hdlc, *ptr);
							
							if (!ret)
							{
								// Now check the user information, if any.
								// compute the remaining data size
								uint16_t remaining_size = (uint16_t)(&buf[size-1] - ptr);
								
								if (remaining_size > 3U)
								{
									// there are some info field, decode it
									ret = hdlc_decode_info_field(hdlc, ++ptr);
								}
							}
						}
					}
				}
				else
				{
					ret = HDLC_ERR_SIZE;
				}
			}
			else
			{
				ret = HDLC_ERR_FORMAT;
			}
		}
	}
	else
	{
		ret = HDLC_ERR_7E;
	}
	return ret;
}

void print_hdlc_result(hdlc_t *hdlc, int code)
{
	if (code == HDLC_OK)
	{
		printf("Success!\r\n");
		
		// Then print HDLC parameters:
		printf("Segmentation: %s\r\n", hdlc->segmentation ? "yes" : "no");
		printf("Size: %d\r\n", hdlc->len);
		printf("Physical address: %d\r\n", hdlc->phy_address);
		printf("Logical device: %d\r\n", hdlc->logical_device);
		printf("Client SAP: %d\r\n", hdlc->client);
		printf("Packet type: %d (%s)", hdlc->type, hdlc_packet_to_string(hdlc));
		printf("Poll/Final bit: %d\r\n", hdlc->poll_final);
	}
	else if (code == HDLC_ERR_7E)
	{
		printf("Packet must start and finish with 0x7E\r\n");
	}
	else if (code == HDLC_ERR)
	{
		printf("Generic error\r\n");
	}
	else if (code == HDLC_ERR_FCS)
	{
		printf("Bad checksum\r\n");
	}
	else
	{
		printf("Unknown error code: %d\r\n", code);
	}
}

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
				print_hdlc_result(&hdlc, ret);
				
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

