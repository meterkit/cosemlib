
#include "aes.h"
#include "unity.h"
#include "unity_fixture.h"


void hexdump(void *ptr, int buflen)
{
    unsigned char *buf = (unsigned char*)ptr;
    int i, j;
    for (i = 0; i < buflen; i += 16)
    {
        for (j=0; j<16; j++)
        {
            if (i+j < buflen)
                printf("%02x ", buf[i+j]);
            else
                printf("   ");
        }
        printf(" ");
        printf("\n");
    }
}


TEST_GROUP(Aes128Gcm);


TEST_SETUP(Aes128Gcm)
{
}

TEST_TEAR_DOWN(Aes128Gcm)
{
}

// Simple NIST test vector of the AES128 GCM cyphering
TEST(Aes128Gcm, NistVector)
{
    const unsigned char key[16]={0xfe,0xff,0xe9,0x92,0x86,0x65,0x73,0x1c,0x6d,0x6a,0x8f,0x94,0x67,0x30,0x83,0x08};
    const unsigned char IV[12] ={0xca,0xfe,0xba,0xbe,0xfa,0xce,0xdb,0xad,0xde,0xca,0xf8,0x88};
    const unsigned char plaintext[]={0xd9,0x31,0x32,0x25,0xf8,0x84,0x06,0xe5,0xa5,0x59,0x09,0xc5,0xaf,0xf5,0x26,0x9a,0x86,0xa7,0xa9,0x53,0x15,0x34,0xf7,0xda,0x2e,0x4c,0x30,0x3d,0x8a,0x31,0x8a,0x72,0x1c,0x3c,0x0c,0x95,0x95,0x68,0x09,0x53,0x2f,0xcf,0x0e,0x24,0x49,0xa6,0xb5,0x25,0xb1,0x6a,0xed,0xf5,0xaa,0x0d,0xe6,0x57,0xba,0x63,0x7b,0x39,0x1a,0xaf,0xd2,0x55};
    const unsigned char add_data[16]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

    unsigned int len_p = sizeof(plaintext);
    unsigned int len_ad = 0;

    unsigned char *ciphertext = malloc(len_p);
    unsigned char tag[16]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

    u8 aes_ctx[AES_PRIV_SIZE];

    int i = aes_gcm_ae(aes_ctx, key, 16, IV, 12, plaintext, len_p, add_data, len_ad, ciphertext, tag);

    TEST_ASSERT_EQUAL(i, 0);

  //  hexdump( ciphertext, len_p);

    // NIST test vector 3, authentication tag must be 4d 5c 2a f3 27 cd 64 a6 2c f3 5a bd 2b a6 fa b4

    static const unsigned char expected[] = { 0x4d, 0x5c, 0x2a, 0xf3, 0x27, 0xcd, 0x64, 0xa6, 0x2c, 0xf3, 0x5a, 0xbd, 0x2b, 0xa6, 0xfa, 0xb4 };
    printf("TAG: "); hexdump( tag, 16);

    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected, tag, 16);

    free(ciphertext);
}

/*
    GreenBook 8.1
  9.2.7.4       HLS authentication mechanisms
  Table 43 – HLS example using authentication-mechanism5 with GMAC


                                    Client                      Server
System Title                    4D4D4D0000000001            4D4D4D0000BC614E
Invocation counter IC               00000001                    01234567
Initialization Vector IV        4D4D4D000000000100000001    4D4D4D0000BC614E01234567
Block cipher key (global) EK    000102030405060708090A0B0C0D0E0F
Authentication Key AK           D0D1D2D3D4D5D6D7D8D9DADBDCDDDEDF
Security control byte SC                    10
Pass 1: Client sends challenge to server CtoS   4B35366956616759 “K56iVagY”
Pass 2: Server sends challenge to client StoC   503677524A323146 “P6wRJ21F”
Pass 3: Client processes StoC SC II AK II StoC  10D0D1D2D3D4D5D6D7D8D9DADBDCDDDEDF503677524A323146

T  =  GMAC (SC  II  AK  II StoC)        1A52FE7DD3E72748973C1E28
f(StoC) = SC II IC II T                 10000000011A52FE7DD3E72748973C1E28

Pass 4: Server processes CtoS
SC II AK II CtoS                        10D0D1D2D3D4D5D6D7D8D9DADBDCDDDEDF4B35366956616759
T (SC II AK II CtoS)                    FE1466AFB3DBCD4F9389E2B7
f(CtoS) = SC II IC II T                 1001234567FE1466AFB3DBCD4F9389E2B7

 */
TEST(Aes128Gcm, GreenBookHlsMechanism5Vector)
{
    // Mechanism 5 is a tag generation with no plain text; the data to authenticate is
    // appended in the AAD: it contains the challenge, the Authentication Key (AK) and the Security Control Byte (SC)

    // PASS 3
    const unsigned char key[16]={0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};
    const unsigned char IV[12] ={0x4D, 0x4D, 0x4D, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01};
    const unsigned char AK[16] = { 0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF };

    unsigned char security_byte = 0x10U;
    const unsigned char stoc[8] = "P6wRJ21F";

    unsigned int len_ad = sizeof(security_byte) + sizeof(AK) + sizeof(stoc);

    unsigned char *aad = malloc(len_ad);

    // prepare AAD buffer
    aad[0] = security_byte;
    memcpy(&aad[1], &AK[0], sizeof(AK));
    memcpy(&aad[1 + sizeof(AK)], &stoc[0], sizeof(stoc));

    unsigned char tag[16]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

  //  u8 aes_ctx[AES_PRIV_SIZE];
  //  int i = aes_gmac(aes_ctx, key, 16, IV, 12, aad, len_ad, tag);

    aes_gcm_ctx gcm_ctx;
    int i = aes_gcm_init(&gcm_ctx, key, 16, IV, 12);
    //aes_gcm_update(&gcm_ctx, NULL, 0, NULL, aad, len_ad);
    aes_gcm_update(&gcm_ctx, NULL, 0, NULL, aad, 16);
    aes_gcm_update(&gcm_ctx, NULL, 0, NULL, &aad[16], 9);
    aes_gcm_finish(&gcm_ctx, tag, 0, len_ad);

    TEST_ASSERT_EQUAL(i, 0);

    static const unsigned char expected[] = { 0x1A,0x52,0xFE,0x7D,0xD3,0xE7,0x27,0x48,0x97,0x3C,0x1E,0x28 };
    printf("TAG: "); hexdump( tag, 16);

    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected, tag, 12); // only the first 12 bytes are used in Cosem

    free(aad);
}

TEST_GROUP_RUNNER(Aes128Gcm)
{
  RUN_TEST_CASE(Aes128Gcm, NistVector);
  RUN_TEST_CASE(Aes128Gcm, GreenBookHlsMechanism5Vector);
}
