
#include "csm_config.h"
#include "csm_association.h"
#include "os.h"

#include <string.h>
#include <stdlib.h>

/*
the  leading  (i.e.  the  leftmost)  64  bits  (8  octets)  shall  hold  the  fixed  field.  It  shall  contain  the
system title, see 4.3.4;
•     the trailing (i.e. the rightmost) 32 bits shall hold the invocation field. The invocation field shall be
an integer counter.
*/

static uint8_t system_title[8] = { 0x4DU, 0x4DU, 0x4DU, 0x00U, 0x00U, 0xBCU, 0x61U, 0x4EU };

static const uint8_t key_kek[16] = { 0xFFU,0xFFU,0xFFU,0xFFU,0xFFU,0xFFU,0xFFU,0xFFU,0xFFU,0xFFU,0xFFU,0xFFU,0xFFU,0xFFU,0xFFU,0xFFU };
static const uint8_t key_guek[16] = { 0xFFU,0xFFU,0xFFU,0xFFU,0xFFU,0xFFU,0xFFU,0xFFU,0xFFU,0xFFU,0xFFU,0xFFU,0xFFU,0xFFU,0xFFU,0xFFU };
static const uint8_t key_gbek[16] = { 0xFFU,0xFFU,0xFFU,0xFFU,0xFFU,0xFFU,0xFFU,0xFFU,0xFFU,0xFFU,0xFFU,0xFFU,0xFFU,0xFFU,0xFFU,0xFFU };
static const uint8_t key_gak[16] = { 0xFFU,0xFFU,0xFFU,0xFFU,0xFFU,0xFFU,0xFFU,0xFFU,0xFFU,0xFFU,0xFFU,0xFFU,0xFFU,0xFFU,0xFFU,0xFFU };

void csm_sys_set_system_title(const uint8_t *buf)
{
    memcpy(system_title, buf, sizeof(system_title));
}

const uint8_t *csm_sys_get_system_title()
{
    return system_title;
}


// External AES128 implementation
int aes_gcm_ad(const u8 *key, size_t key_len, const u8 *iv, size_t iv_len,
           const u8 *crypt, size_t crypt_len,
           const u8 *aad, size_t aad_len, const u8 *tag, u8 *plain);

int csm_sys_gcm_ad(csm_sec_key key_id,
                   uint8_t *crypted, uint32_t crypted_len,
                   const uint8_t *aad, uint32_t aad_len,
                   uint8_t *tag,
                   uint8_t *plain,
                   uint32_t ic)
{
    const uint8_t *key = NULL;
    uint8_t iv[12];

    memcpy(&iv[0], &system_title[0], sizeof(system_title));
    PUT_BE32(&system_title[8], ic);

    switch(key_id)
    {
    case CSM_SEC_KEK:
        key = key_kek;
        break;
    case CSM_SEC_GUEK:
        key = key_guek;
        break;
    case CSM_SEC_GBEK:
        key = key_gbek;
        break;
    default:
    case CSM_SEC_GAK:
        key = key_gak;
        break;
    }

    return aes_gcm_ad(key, 16, iv, 12, crypted, crypted_len, aad, aad_len, tag, plain);
}


struct cfg_cosem
{
    uint8_t sap; //!< Sap number of the association
    uint8_t lls_password[CSM_DEF_LLS_SIZE]; // Password.
    uint8_t mechanism_id;
};

// FIXME: in a real server, store these parameters into non-volatile memory (get/set access)
static const struct cfg_cosem cfg_cosem_passwords [] =
{
    { 16U, { 0x30U, 0x30U, 0x30U, 0x30U, 0x30U, 0x30U, 0x30U, 0x30U }, CSM_AUTH_LOWEST_LEVEL},  // Public
    { 1U, { 0x30U, 0x30U, 0x30U, 0x30U, 0x30U, 0x30U, 0x30U, 0x31U }, CSM_AUTH_LOW_LEVEL}, // Management
};

#define CFG_COSEM_NB_ASSOS  (sizeof(cfg_cosem_passwords)/sizeof(struct cfg_cosem))

int csm_sys_test_lls_password(uint8_t sap, uint8_t *buf)
{
    int valid = FALSE;
    for (uint32_t i = 0U; i < CFG_COSEM_NB_ASSOS; i++)
    {
        if (sap == cfg_cosem_passwords[i].sap)
        {
            int ret = memcmp(buf, cfg_cosem_passwords[i].lls_password, CSM_DEF_LLS_SIZE);
            valid = (ret == 0) ? TRUE : FALSE;
            break;
        }
    }
    return valid;
}

uint8_t csm_sys_get_mechanism_id(uint8_t sap)
{
    uint8_t mechanism_id = CSM_AUTH_LOWEST_LEVEL;
    for (uint32_t i = 0U; i < CFG_COSEM_NB_ASSOS; i++)
    {
        if (sap == cfg_cosem_passwords[i].sap)
        {
            mechanism_id = cfg_cosem_passwords[i].mechanism_id;
            break;
        }
    }
    return mechanism_id;
}

uint8_t csm_sys_get_random_u8()
{
    return (uint8_t)(rand()%256);
}

