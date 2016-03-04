#ifndef AXDR_CODEC_H
#define AXDR_CODEC_H

#include "csm_array.h"
#include "csm_definitions.h"

enum axdr_tag
{
    AXDR_TAG_NULL   =   0x00U,
};

/*
static const std::uint8_t cSequenceOf	 = 0x01U;
static const std::uint8_t cSequence		 = 0x02U;
static const std::uint8_t cBoolean		 = 0x03U;
static const std::uint8_t cBitString	 = 0x04U;
static const std::uint8_t cInteger32	 = 0x05U;
static const std::uint8_t cUnsigned32	 = 0x06U;
static const std::uint8_t cFloatingPoint = 0x07U;
static const std::uint8_t cOctetsString	 = 0x09U;
static const std::uint8_t cVisibleString = 0x0AU;
static const std::uint8_t cUtf8String    = 0x0BU;
static const std::uint8_t cBcd			 = 0x0DU;
static const std::uint8_t cInteger8		 = 0x0FU;
static const std::uint8_t cInteger16	 = 0x10U;
static const std::uint8_t cUnsigned8	 = 0x11U;
static const std::uint8_t cUnsigned16	 = 0x12U;
static const std::uint8_t cCompactArray	 = 0x13U;
static const std::uint8_t cInteger64	 = 0x14U;
static const std::uint8_t cUnsigned64	 = 0x15U;
static const std::uint8_t cEnumerated	 = 0x16U;
static const std::uint8_t cFloat32		 = 0x17U;
static const std::uint8_t cFloat64		 = 0x18U;
static const std::uint8_t cUtc			 = 0x19U;
static const std::uint8_t cDate			 = 0x1AU;
static const std::uint8_t cTime			 = 0x1BU;
*/

int axdr_decode_null(csm_array *array);

#endif // AXDR_CODEC_H
