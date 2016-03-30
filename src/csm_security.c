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



csm_sec_result csm_sec_auth_decrypt(csm_array *array, csm_sec_control_byte sc)
{
    (void) array;
    (void) sc;
    /*
    if (sc.sh_bit_field.encryption)
    {

    }

    uint8_t aad[];

    // decrypt/verify in place
    csm_sys_gcm_ad(CSM_SEC_GUEK, csm_array_rd_data(array), array->rd_index, )
    */

    return CSM_SEC_OK;
}
