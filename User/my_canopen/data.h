#ifndef __data_h__
#define __data_h__

#include "applicfg.h"
#include "my_canopen_adapter.h"
#include "objdictdef.h"
#include "sdo.h"
#include "bsp_can.h"
#include "los_typedef.h"
#include "objacces.h"

#define SDO_MAX_SIMULTANEOUS_TRANSFERS 40


struct struct_CO_Data {
	UNS8 *bDeviceNodeId;
	const indextable *objdict;
	const quick_index *firstIndex;
	const quick_index *lastIndex;
	const UNS16 *ObjdictSize;
	const UNS8 *iam_a_slave;
	valueRangeTest_t valueRangeTest;
	
	s_transfer transfers[SDO_MAX_SIMULTANEOUS_TRANSFERS];
	
	UNS8 toggle;
	CAN_PORT	canHandle;
	scanIndexOD_t scanIndexOD;
	storeODSubIndex_t storeODSubIndex;
};


#define s_transfer_Initializer {\
	0,            /* nodeId */\
	0,            /* wohami */\
	0,            /* toggle */\
	0,            /* obortCode */\
	0,            /* index */\
	0,            /* subIndex */\
	0,            /* count */\
	0,            /* offset */\
	{0},          /* data */\
	0,            /* dataType */\
	NULL          /* Callback */\
},

#define CANOPEN_NODE_DATA_INITIALIZER(NODE_PREFIX){\
	/* Object dictionary*/\
	& NODE_PREFIX ## _bDeviceNodeId,       /* bDeviceNodeId */\
	NODE_PREFIX ## _objdict,               /* objdict */\
	& NODE_PREFIX ## _firstIndex,          /* firstIndex */\
	& NODE_PREFIX ## _lastIndex,           /* lastIndex */\
	& NODE_PREFIX ## _ObjdictSize,         /* ObjdictSize */\
	& NODE_PREFIX ## _iam_a_slave,         /* iam_a_slave */\
	NODE_PREFIX ## _valueRangeTest,        /* valueRangeTest */\
	\
	/* SDO, structure s_transfer */\
	{\
		REPEAT_SDO_MAX_SIMULTANEOUS_TRANSFERS_TIMES(s_transfer_Initializer)\
	},\
	\
	/* General */\
	0,                                    /* toggle */\
	NULL,                                  /* canSend */\
	NODE_PREFIX ## _scanIndexOD,          /* scanIndexOD */\
	_storeODSubIndex,                     /* storeODSubIndex */\
}


#endif

