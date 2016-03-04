#include "csm_association.h"
#include "string.h"
#include "csm_axdr_codec.h"

// Since this is part of a Cosem stack, simplify the decoding to lower code & RAM ;
// Instead of performing a real decoding, just compare the memory as it is always the same
static const uint8_t cOidHeader[] = {0x60U, 0x85U, 0x74U, 0x05U, 0x08U};

// Object identifier names
#define APP_CONTEXT_NAME            1U
#define SECURITY_MECHANISM_NAME     2U


typedef enum
{
    CSM_ACSE_ERR = 0U, ///< Encoding/decoding is NOT good, stop here if it is required
    CSM_ACSE_OK = 1U    ///< Encoding/decoding is good, we can continue

} csm_acse_code;

typedef csm_acse_code (*extract_param)(csm_asso_state *state, csm_ber *ber, csm_array *array);
typedef csm_acse_code (*insert_param)(csm_asso_state *state, csm_ber *ber, csm_array *array);


static csm_acse_code acse_aarq_decoder(csm_asso_state *state, csm_ber *ber, csm_array *array)
{
    csm_acse_code ret = CSM_ACSE_OK;
    (void) state;

    CSM_LOG("[ACSE] Found AARQ tag");

    // Control the length
    if (ber->length.length != csm_array_remaining(array))
    {
        CSM_ERR("[ACSE] Bad AARQ size");
        ret = CSM_ACSE_ERR;
    }

    return ret;
}

static csm_acse_code acse_app_context_decoder(csm_asso_state *state, csm_ber *ber, csm_array *array)
{
    csm_acse_code ret = CSM_ACSE_ERR;
    (void) state;
    (void) array;

    CSM_LOG("[ACSE] Found APPLICATION CONTEXT tag");

    // the length of the object identifier must be 7 bytes + 2 bytes for the BER header = 9 bytes
    if (ber->length.length == 9U)
    {
        ret = CSM_ACSE_OK;
    }
    else
    {
        CSM_ERR("[ACSE] Bad object identifier size");
    }

    return ret;
}

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
static csm_acse_code acse_oid_decoder(csm_asso_state *state, csm_ber *ber, csm_array *array)
{
    csm_acse_code ret = CSM_ACSE_ERR;
    (void) ber;

    CSM_LOG("[ACSE] Found OBJECT IDENTIFIER tag");

    ber_object_identifier oid;
    oid.header = cOidHeader;
    oid.size = 5U;

    if (csm_ber_decode_object_identifier(&oid, array) == TRUE)
    {
        // in the case of LN referencing, with no ciphering: 2, 16, 756, 5, 8, 1, 1;
        // in the case of SN referencing, with no ciphering: 2, 16, 756, 5, 8, 1, 2;

        if ((oid.name == APP_CONTEXT_NAME))
        {
            switch (oid.id)
            {
                case LN_REF:
                    state->ref = LN_REF;
                    ret = CSM_ACSE_OK;
                    CSM_LOG("[ACSE] LogicalName referencing");
                    break;
                case SN_REF:
                    state->ref = SN_REF;
                    ret = CSM_ACSE_OK;
                    CSM_LOG("[ACSE] ShortName referencing");
                    break;
                default:
                    CSM_LOG("[ACSE] Referencing not supported");
                    break;
            }
        }
        // in the case of low-level-security: 2, 16, 756, 5, 8, 2, 1;
        // in the case of high-level-security (5): 2, 16, 756, 5, 8, 2, 5;
        else if ((oid.name == SECURITY_MECHANISM_NAME))
        {
            switch (oid.id)
            {
                case LOW_LEVEL:
                    state->auth_level = LOW_LEVEL;
                    ret = CSM_ACSE_OK;
                    CSM_LOG("[ACSE] Low level authentication");
                    break;
                case HIGH_LEVEL:
                    state->auth_level = HIGH_LEVEL;
                    ret = CSM_ACSE_OK;
                    CSM_LOG("[ACSE] High level authentication");
                    break;
                default:
                    CSM_LOG("[ACSE] Authentication level not supported");
                    break;
            }
        }
    }
    else
    {
        CSM_ERR("[ACSE] Bad Object Identifier contents or size");
    }

    return ret;
}


static csm_acse_code acse_req_decoder(csm_asso_state *state, csm_ber *ber, csm_array *array)
{
    csm_acse_code ret = CSM_ACSE_ERR;
    (void) state;

    CSM_LOG("[ACSE] Found sender requirements tag");

    if (ber->length.length == 2U)
    {
        // encoding of the authentication functional unit (0)
        // NOTE The number of bits coded may vary from client to client, but
        // within the COSEM environment, only bit 0 set to 1 (indicating the
        // requirement of the authentication functional unit) is to be respected.
        uint8_t byte;
        if (csm_array_read_u8(array, &byte))
        {
            // encoding of the number of unused bits in the last byte of the BIT STRING
            if (byte == 0x07U)
            {
                if (csm_array_read_u8(array, &byte))
                {
                    if (byte == 0x80U)
                    {
                        ret = CSM_ACSE_OK;
                    }
                }
            }
        }
    }
    else
    {
        CSM_ERR("[ACSE] Sender requirements bad size");
    }

    return ret;
}

static csm_acse_code acse_auth_value_decoder(csm_asso_state *state, csm_ber *ber, csm_array *array)
{
    csm_acse_code ret = CSM_ACSE_ERR;

    if (ber->length.length == (SIZE_OF_AUTH_VALUE + 2U))
    {
        CSM_LOG("[ACSE] Found authentication value tag");
        if (csm_ber_decode(ber, array))
        {
            if (ber->length.length == SIZE_OF_AUTH_VALUE)
            {
                if (ber->tag.tag == (TAG_CONTEXT_SPECIFIC))
                {
                    // It is a GraphicString
                    if (csm_array_read_buff(array, state->auth_value, SIZE_OF_AUTH_VALUE))
                    {
                        ret = CSM_ACSE_OK;
                    }
                }
            }

            if (ret == CSM_ACSE_ERR)
            {
                CSM_ERR("[ACSE] Bad authentication value size");
            }
        }
        else
        {
            CSM_ERR("[ACSE] Bad authentication value format");
        }
    }
    else
    {
        CSM_ERR("[ACSE] Bad authentication value size");
    }

    return ret;
}


static csm_acse_code acse_user_info_decoder(csm_asso_state *state, csm_ber *ber, csm_array *array)
{
    csm_acse_code ret = CSM_ACSE_ERR;

    CSM_LOG("[ACSE] Found user info tag");
    if (csm_ber_decode(ber, array))
    {
        if (ber->tag.id == BER_TYPE_OCTET_STRING)
        {
            // Now decode the A-XDR encoded packet
            uint8_t byte;
            if(csm_array_read_u8(array, &byte))
            {
                if (byte == AXDR_INITIATE_REQUEST)
                {

                    /*
                    -- xDLMS APDU-s used during Association establishment
                    InitiateRequest ::= SEQUENCE
                    {
                    --  shall not be encoded in DLMS without ciphering
                    dedicated-key                      OCTET STRING OPTIONAL,
                    response-allowed                   BOOLEAN DEFAULT TRUE,
                    proposed-quality-of-service        [0] IMPLICIT Integer8 OPTIONAL,
                    proposed-dlms-version-number       Unsigned8,
                    proposed-conformance               Conformance, -- Shall be encoded in BER
                    client-max-receive-pdu-size        Unsigned16
                    }

                    -- The Conformance field shall be encoded in BER. See IEC 61334-6 Example 1.

                    */

                    CSM_LOG("[ACSE] Found xDLMS InitiateRequest encoded APDU");
                    if(csm_array_read_u8(array, &byte))
                    {
                        if (byte != AXDR_TAG_NULL)
                        {
                            // FIXME: copy the dedicated key
                        }
                    }

                    int valid = axdr_decode_null(array); //  response-allowed
                    valid =  valid && axdr_decode_null(array); // proposed_quality_of_service

                    // proposed-dlms-version-number: always 6
                    valid = valid && csm_array_read_u8(array, &byte);
                    valid = valid && (byte == 6U ? TRUE : FALSE);

                    // conformance, [APPLICATION 31] IMPLICIT BIT STRING
                    // encoding of the [APPLICATION 31] tag (ASN.1 explicit tag)
                    valid = valid && csm_ber_decode(ber, array);
                    if ((ber->tag.tag == 0x5FU) && (ber->tag.ext == 31U))
                    {
                        if (ber->length.length == 4U)
                        {
                            valid = valid && csm_array_read_u8(array, &byte);
                            valid = valid && (byte == 0U ? TRUE : FALSE); // unused bits in the bitstring

                            state->proposed_conformance = ((uint32_t)byte) << 16U;
                            valid = valid && csm_array_read_u8(array, &byte);
                            state->proposed_conformance += ((uint32_t)byte) << 8U;
                            valid = valid && csm_array_read_u8(array, &byte);
                            state->proposed_conformance += ((uint32_t)byte);

                            valid = valid && csm_array_read_u16(array, &state->client_max_receive_pdu_size);
                        }
                    }
                    else
                    {
                        valid = 0U;
                    }

                    if (valid)
                    {
                        ret = CSM_ACSE_OK;
                    }
                }
            }
        }
    }

    return ret;
}


static csm_acse_code acse_skip_decoder(csm_asso_state *state, csm_ber *ber, csm_array *array)
{
    (void) state;
    if (ber->tag.isPrimitive)
    {
        // This BER contains data that is not managed
        // advance the pointer to the next BER header
        (void) csm_array_jump(array, ber->length.length);
    }
    CSM_LOG("[ACSE] Skipped tag: %d", ber->tag.tag);
    return CSM_ACSE_OK;
}

// -------------------------------   ENCODERS ------------------------------------------

static csm_acse_code acse_aare_encoder(csm_asso_state *state, csm_ber *ber, csm_array *array)
{

}


typedef struct
{
    uint8_t tag;
    uint8_t optional;
    extract_param extract_func;
    insert_param insert_func;
} csm_asso_codec;

static const csm_asso_codec aarq_codec_chain[] =
{
    {CSM_ASSO_AARQ, FALSE, acse_aarq_decoder, NULL},
    {CSM_ASSO_PROTO_VER, TRUE, acse_skip_decoder, NULL},
    {CSM_ASSO_APP_CONTEXT_NAME, FALSE, acse_app_context_decoder, NULL},
    {BER_TYPE_OBJECT_IDENTIFIER, FALSE, acse_oid_decoder, NULL},
    {CSM_ASSO_CALLED_AP_TITLE, TRUE, acse_skip_decoder, NULL},
    {CSM_ASSO_CALLED_AE_QUALIFIER, TRUE, acse_skip_decoder, NULL},
    {CSM_ASSO_CALLED_AP_INVOC_ID, TRUE, acse_skip_decoder, NULL},
    {BER_TYPE_INTEGER, TRUE, acse_skip_decoder, NULL},
    {CSM_ASSO_CALLED_AE_INVOC_ID, TRUE, acse_skip_decoder, NULL},
    {BER_TYPE_INTEGER, TRUE, acse_skip_decoder, NULL},
    {CSM_ASSO_CALLING_AP_TITLE, TRUE, acse_skip_decoder, NULL},
    {CSM_ASSO_CALLING_AE_QUALIFIER, TRUE, acse_skip_decoder, NULL},
    {CSM_ASSO_CALLING_AP_INVOC_ID, TRUE, acse_skip_decoder, NULL},
    {BER_TYPE_INTEGER, TRUE, acse_skip_decoder, NULL},
    {CSM_ASSO_CALLING_AE_INVOC_ID, TRUE, acse_skip_decoder, NULL},
    {BER_TYPE_INTEGER, TRUE, acse_skip_decoder, NULL},
    {CSM_ASSO_SENDER_ACSE_REQU, TRUE, acse_req_decoder, NULL},
    {CSM_ASSO_MECHANISM_NAME, TRUE, acse_oid_decoder, NULL},
    {CSM_ASSO_CALLING_AUTH_VALUE, TRUE, acse_auth_value_decoder, NULL},
    {CSM_ASSO_IMPLEMENTATION_INFO, TRUE, acse_skip_decoder, NULL},
    {CSM_ASSO_USER_INFORMATION, TRUE, acse_user_info_decoder, NULL}
};



#define CSM_ACSE_AARQ_CHAIN_SIZE   (sizeof(aarq_codec_chain)/sizeof(csm_asso_codec))

static const csm_asso_codec aare_codec_chain[] =
{
    {CSM_ASSO_AARE, FALSE, NULL, acse_aare_encoder},
};

#define CSM_ACSE_AARE_CHAIN_SIZE   (sizeof(aarq_codec_chain)/sizeof(csm_asso_codec))

void csm_asso_init(csm_asso_state *state)
{
    state->state_cf = CF_IDLE;
    state->auth_level = NO_LEVEL;
    state->ref = NO_REF;
}

// Check is association is granted
int csm_asso_is_granted(csm_asso_state *state)
{
    return TRUE;
}

int csm_asso_decoder(csm_asso_state *asso, csm_array *packet)
{
    csm_ber ber;
    uint8_t decoder_index = 0U;

    // Decode first bytes
    int ret = csm_ber_decode(&ber, packet);

    do
    {
        if (ret)
        {
            const csm_asso_codec *codec = &aarq_codec_chain[decoder_index];
            decoder_index++;
            if (ber.tag.tag == codec->tag)
            {
                ret = FALSE;
                if ((codec->extract_func != NULL))
                {
                    ret = codec->extract_func(asso, &ber, packet);
                }

                if (codec->optional && !ret)
                {
                    ret = TRUE; // normal error (optional field)
                }

                if ((ret) && (decoder_index < CSM_ACSE_AARQ_CHAIN_SIZE))
                {
                    // Continue decoding BER
                    ret = csm_ber_decode(&ber, packet);
                }
            }
        }

        if (!ret)
        {
            break;
        }
    }
    while (decoder_index < CSM_ACSE_AARQ_CHAIN_SIZE);

    return ret;
}

int csm_asso_execute(csm_asso_state *asso, csm_array *packet)
{
    int bytes_to_reply = 0;

    if (asso->state_cf  == CF_IDLE)
    {
        if (csm_asso_decoder(asso, packet))
        {
            if (csm_asso_is_granted(asso))
            {
                // Send AARE
                CSM_LOG("[ACSE] Access granted!");
            }
            else
            {
                // FIXME: detect reason if reject
                // FIXME: send an error
                CSM_ERR("[ACSE] Connection rejected, reason: ");
            }
        }
        else
        {
            CSM_ERR("[ACSE] BER decoding error");
        }
    }

    return bytes_to_reply;
}
