#ifndef CSM_BER_H
#define CSM_BER_H

#include <stdint.h>
#include "csm_array.h"

enum
{
    CLASS_MASK	= 0xC0,
    TYPE_MASK	= 0x20, //< Bit 6 indicates if it is a Primitive (0) of Constructed (1)
    TAG_MASK	= 0x1F,
    LEN_XTND	= 0x80,
    LEN_MASK	= 0x7F
};

/*
    Class           bit 8 	bit 7 	description
Universal           0       0       The type is native to ASN.1
Application         0       1   	The type is only valid for one specific application
Context-specific 	1       0       Meaning of this type depends on the context (such as within a sequence, set or choice)
Private             1       1       Defined in private specifications
*/
enum tag_type
{
    TAG_UNIVERSAL           = 0x00U,
    TAG_APPLICATION         = 0x40U,
    TAG_CONTEXT_SPECIFIC    = 0x80U,
    TAG_PRIVATE             = 0xC0U
};

typedef struct
{
    uint8_t cls;
    uint8_t isPrimitive;
    uint8_t id;
    uint8_t tag;    //< We limit to one-byte tag
    uint8_t nbytes;
} ber_tag;

typedef struct
{
    uint16_t length; //< Cosem APDU size is coded on Unsigned16
    uint8_t nbytes;
} ber_length;

struct ber_struct
{
    ber_tag tag;
    ber_length length;
    unsigned int nbytes;
    unsigned int depth;
    uint8_t *value;
    struct ber_struct *next;
};

/* Green Book 8
9.4.2.2       Registered COSEM names

    Within an OSI  environment, many different types  of network objects must be  identified  with globally
    unambiguous   names.   These   network   objects   include   abstract   syntaxes,   transfer   syntaxes,
    application  contexts,  authentication  mechanism  names,  etc.  Names  for  these  objects  in  most  cases
    are  assigned  by  the  committee  developing  the  particular  basic  ISO  standard  or  by  implementers’
    workshops,  and  should  be  registered.  For  DLMS/COSEM,  these  object  names  are  assigned  by  the
    DLMS UA, and are specified below.

    The  decision  no.  1999.01846  of  OFCOM,  Switzerland,  attributes  the  following  prefix  for  object
    identifiers specified by the DLMS User Association.

    { joint-iso-ccitt(2) country(16) country-name(756) identified-organisation(5) DLMS-UA(8) }

    For DLMS/COSEM, object identifiers are specified for naming the following items:
        --> COSEM application context names;
        --> COSEM authentication mechanism names;
        --> cryptographic algorithm ID-s.
*/
typedef struct
{
    uint8_t header[5];  //< 5 bytes values representing the Organisation ID, always  60 85 74 05 08 for Cosem (see chapter 11.4 page 398)
    uint8_t name;       //< name object
    uint8_t id;         //< id object
} ber_object_identifier;

typedef struct ber_struct csm_ber;

void csm_ber_dump(csm_ber *i_ber);
int csm_ber_decode_object_identifier(ber_object_identifier *oid, csm_array *array);
int csm_ber_decode(csm_ber *ber, csm_array *array);

#endif // CSM_BER_H
