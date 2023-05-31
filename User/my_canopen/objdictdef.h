#ifndef __objdictdef_h__
#define __objdictdef_h__

#include "applicfg.h"

typedef struct td_subindex
{
	UNS8    bDataType;
	UNS32	size;
	void 	*pObject;
}subindex;

typedef struct td_indextable
{
	subindex *pSubindex;
	UNS8	bSubCount;
	UNS16	index;
}indextable;

typedef struct s_quick_index{
	UNS16	SDO_SVR;
	UNS16	SDO_CLT;
}quick_index;

typedef UNS32 (*ODCallback_t)(CO_Data* d, const indextable *, UNS8 bSubindex);
typedef const indextable *(*scanIndexOD_t)(UNS16 wIndex, UNS32 *errorCode, ODCallback_t **Callback);

#define boolean         0x01
#define int8            0x02
#define int16           0x03
#define int32           0x04
#define uint8           0x05
#define uint16          0x06
#define uint32          0x07
#define real32          0x08
#define visible_string  0x09
#define octet_string    0x0A
#define unicode_string  0x0B
#define time_of_day     0x0C
#define time_difference 0x0D

#define domain          0x0F
#define int24           0x10
#define real64          0x11
#define int40           0x12
#define int48           0x13
#define int56           0x14
#define int64           0x15
#define uint24          0x16

#define uint40          0x18
#define uint48          0x19
#define uint56          0x1A
#define uint64          0x1B

#endif

