#ifndef __sdo_h__
#define __sdo_h__

#include "applicfg.h"
#include "data.h"
#include "my_canopen_adapter.h"

typedef void (*SDOCallback_t)(CO_Data *d, UNS8 nodeId);

struct struct_s_transfer {
	UNS8	CliServNbr;
	
	UNS8	whoami;
	UNS8	state;
	UNS8	toggle;
	UNS32	abortCode;
	UNS16	index;
	UNS8	subIndex;
	UNS32	count;
	UNS32	offset;
	
	UNS8	data[SDO_MAX_LENGTH_TRANSFER];

	UNS8 dataType;
	
	SDOCallback_t Callback;
};

typedef struct struct_s_transfer s_transfer;

void resetSDO(CO_Data *d);

UNS32 SDOlineToObjdict(CO_Data *d, UNS8 line);

UNS32 objdictToSDOline (CO_Data* d, UNS8 line);

UNS8 lineToSDO(CO_Data *d, UNS8 line, UNS32 nbBytes, UNS8 *data);

UNS8 SDOtoLine(CO_Data *d, UNS8 line, UNS32 nbBytes, UNS8 *data);

UNS8 failedSDO(CO_Data *d, UNS8 CliServNbr, UNS8 whoami, UNS16 index, UNS8 subIndex, UNS32 abortCode);

void resetSDOline(CO_Data *d, UNS8 line);

UNS8 initSDOline(CO_Data *d, UNS8 line, UNS8 CliServNbr, UNS16 index, UNS8 subIndex, UNS8 state);

UNS8 getSDOfreeLine(CO_Data *d, UNS8 whoami, UNS8 *line);

UNS8 getSDOlineOnUse(CO_Data *d, UNS8 CliServNbr, UNS8 whoami, UNS8 *line);

UNS8 getSDOlineToClose(CO_Data *d, UNS8 CliServNbr, UNS8 whoami, UNS8 *line);

UNS8 closeSDOTransfer(CO_Data *d, UNS8 CliServNbr, UNS8 whoami);

UNS8 getSDOlineRestBytes(CO_Data * d, UNS8 line, UNS32 * nbBytes);

#endif
