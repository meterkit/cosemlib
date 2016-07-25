/**
 * AXDR utility function to serialize data
 *
 * Copyright (c) 2016, Anthony Rabine
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the BSD license.
 * See LICENSE.txt for more details.
 *
 */

#ifndef AXDR_CODEC_H
#define AXDR_CODEC_H

#include "csm_array.h"
#include "csm_definitions.h"

/*
Data ::= CHOICE
{
null-data                          [0]   IMPLICIT   NULL,
array                              [1]   IMPLICIT   SEQUENCE OF Data,
structure                          [2]   IMPLICIT   SEQUENCE OF Data,
boolean                            [3]   IMPLICIT   BOOLEAN,
bit-string                         [4]   IMPLICIT   BIT STRING,
double-long                        [5]   IMPLICIT   Integer32,
double-long-unsigned               [6]   IMPLICIT   Unsigned32,
octet-string                       [9]   IMPLICIT   OCTET STRING,
visible-string                     [10]  IMPLICIT   VisibleString,
utf8-string                        [12]  IMPLICIT   UTF8String,
bcd                                [13]  IMPLICIT   Integer8,
integer                            [15]  IMPLICIT   Integer8,
long                               [16]  IMPLICIT   Integer16,
unsigned                           [17]  IMPLICIT   Unsigned8,
long-unsigned        [18]  IMPLICIT   Unsigned16,
compact-array                      [19]  IMPLICIT   SEQUENCE
{
contents-description                [0]              TypeDescription,
array-contents                      [1]   IMPLICIT   OCTET STRING
},
long64                             [20]  IMPLICIT   Integer64,
long64-unsigned                    [21]  IMPLICIT   Unsigned64,
enum                               [22]  IMPLICIT   Unsigned8,
float32                            [23]  IMPLICIT   OCTET STRING (SIZE(4)),
float64                            [24]  IMPLICIT   OCTET STRING (SIZE(8)),
date-time                          [25]  IMPLICIT   OCTET STRING (SIZE(12)),
date                               [26]  IMPLICIT   OCTET STRING (SIZE(5)),
time                               [27]  IMPLICIT   OCTET STRING (SIZE(4)),
dont-care                          [255] IMPLICIT   NULL
}
*/

enum axdr_tag
{
    AXDR_TAG_NULL           = 0U,
    AXDR_TAG_ARRAY          = 1U,
    AXDR_TAG_STRUCTURE      = 2U,
    AXDR_TAG_BOOLEAN        = 3U,
    AXDR_TAG_OCTETSTRING    = 9U
};

// Decoders
int csm_axdr_rd_null(csm_array *array);
int csm_axdr_rd_octetstring(csm_array *array);

// Encoders
int csm_axdr_wr_octetstring(csm_array *array, const uint8_t *buffer, uint32_t size);
int csm_axdr_wr_boolean(csm_array *array, uint8_t value);

#endif // AXDR_CODEC_H
