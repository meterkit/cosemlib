#ifndef CSM_DATA_ID_GEN_H_
#define CSM_DATA_ID_GEN_H_

#include <stdint.h>

#define cNumberOfAttributeBits		6U // Number of bits used to encode the attribute number for Internal ID automatic generation
#define cAttributeMask  			0x0000003FU // Use it to filter-out the attribute in the Internal ID
#define cComponentDBMask  			0xFC000000U // Use it to filter-out the Component DB in the Internal ID


#define ATTRIB0_DB	0x00U
#define CLOCK_DB 	0x03U


#define cClockClockIdCurrentDateTime  			0x0C300002U		//OBIS code:0.0.1.0.0.255, attr: 2
#define cClockClockIdTimeZone  					0x0C300043U		//OBIS code:0.0.1.0.0.255, attr: 3
#define cClockClockIdStatus  					0x0C300084U		//OBIS code:0.0.1.0.0.255, attr: 4
#define cClockClockIdDstBegin  					0x0C3000C5U		//OBIS code:0.0.1.0.0.255, attr: 5
#define cClockClockIdDstEnd  					0x0C300106U		//OBIS code:0.0.1.0.0.255, attr: 6
#define cClockClockIdDstDeviation  				0x0C300147U		//OBIS code:0.0.1.0.0.255, attr: 7
#define cClockClockIdDstEnable  				0x0C300188U		//OBIS code:0.0.1.0.0.255, attr: 8
#define cClockClockIdHardClockParameters  		0x0C3001C9U		//OBIS code:0.0.1.0.0.255, attr: 9
#define cClockClockTimeShiftLimitIdValue  		0x0C300202U		//OBIS code:1.0.0.9.11.255, attr: 2
#define cClockClockTimeShiftLimitIdScalerUnit  	0x0C300243U		//OBIS code:1.0.0.9.11.255, attr: 3


#define cClockClockAttribute0  0x00500000U



#endif // CSM_DATA_ID_GEN_H_

