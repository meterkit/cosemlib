/**
 * Copyright (c) 2016, Anthony Rabine
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the BSD license.
 * See LICENSE.txt for more details.
 *
 * API of functions that must be implemented outside the main DLMS/Cosem stack.
 * This is out king of HAL.
 */

#ifndef CSM_SYSTEM_H
#define CSM_SYSTEM_H

#include <stdint.h>

/* GreenBook 8
System title 4.3.4
The system title Sys-T shall  uniquely  identify  each DLMS/COSEM  entity  that may  be  server,  a client
or a third party that can access servers via clients. The system title:
  * shall be 8 octets long;
  * shall be unique.
The  leading  (i.e.,  the  3  leftmost)  octets  should  hold  the  three-letter  manufacturer  ID1.  This  is  the
same as the leading three octets of the Logical Device Name, see 4.3.5. The remaining 5 octets shall
ensure uniqueness.
*/

// This function makes a copy of the system title locally in RAM
void csm_sys_set_system_title(const uint8_t *buf);

// This function is copying the system title to the buffer passed in reference
void csm_sys_get_system_title(uint8_t *buf);
void csm_sys_get_init_vector(uint8_t *buf, uint32_t counter);


enum csm_key
{
    KEK,    //!< the master key
    GUEK,   //!< global unicast encryption key GUEK
    GBEK,   //!< global broadcast encryption key GBEK
    GAK,    //!< (global) authentication key, GAK
};

void csm_sys_get_key(enum csm_key key, uint8_t *buf);


#endif // CSM_SYSTEM_H
