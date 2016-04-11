/**
 * Cosem security layer functions to (de)cypher and authenticate packets
 * ---------------------------------------------------------------------------------
 * Copyright (c) 2016, Anthony Rabine
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the BSD license.
 * See LICENSE.txt for more details.
 *
 */


#include "csm_security.h"
#include "csm_definitions.h"



csm_sec_result csm_sec_auth_decrypt(csm_array *array, csm_array *aad)
{
    csm_sec_control_byte sc;

    /*
    csm_array_get(array, &sc.sh_byte);

    if (sc.sh_bit_field.encryption)
    {

    }

    if (sc.sh_bit_field.authentication)
    {

    }

    int csm_sys_gcm_init(uint8_t channel, csm_sec_key key_id, uint32_t ic, const uint8_t *aad, uint32_t aad_len);
    int csm_sys_gcm_update(uint8_t channel, const uint8_t *plain, uint32_t plain_len,
                           uint8_t *crypt);
    int csm_sys_gcm_finish(uint8_t channel, uint8_t *tag);


    uint8_t aad[];

    // decrypt/verify in place
    csm_sys_gcm_ad(CSM_SEC_GUEK, csm_array_rd_data(array), array->rd_index, )

*/
    return CSM_SEC_OK;
}
