#include "csm_association.h"

// Since this is part of a Cosem stack, simplify the decoding to lower code & RAM ;
// Instead of performing a real decoding, just compare the memory as it is always the same
static const uint8_t cOidHeader[] = {0x60U, 0x85U, 0x74U, 0x05U, 0x08U};


/*
Conformance ::= [APPLICATION 31] IMPLICIT BIT STRING
{

}
*/



void csm_asso_init(csm_asso_state *state)
{
    state->state_cf = CF_IDLE;
}

int csm_asso_execute(csm_asso_state *state, csm_array *packet)
{
    int ret = FALSE;
    (void) state;

    csm_ber ber;
    while (csm_ber_decode(&ber, packet));
    return ret;
}
