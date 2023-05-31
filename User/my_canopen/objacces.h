#ifndef __objacces_h__
#define __objacces_h__

#include "applicfg.h"

typedef UNS32 (*valueRangeTest_t)(UNS8 typeValue, void *Value);
typedef void (*storeODSubIndex_t)(CO_Data *d, UNS16 wIndex, UNS8 bSubindex, void *ptr, UNS32 size);
void _storeODSubIndex(CO_Data *d, UNS16 wIndex, UNS8 bSubindex, void *ptr, UNS32 size);
UNS32 _setODentry(CO_Data * d, UNS16 wIndex, UNS8 bSubindex, void * pSourceData, UNS32 * pExpectedSize, UNS8 checkAccess, UNS8 endianize);
UNS32 _getODentry(CO_Data * d, UNS16 wIndex, UNS8 bSubindex, void * pDestData, UNS32 * pExpectedSize, UNS8 * pDataType, UNS8 checkAccess, UNS8 endianize);


#define setODentry( d, wIndex, bSubindex, pSourceData, pExpectedSize, \
                  checkAccess) \
       _setODentry( d, wIndex, bSubindex, pSourceData, pExpectedSize, \
                  checkAccess, 1)


#define getODentry(OD, wIndex, bSubindex, pDestData, pExpectedSize, \
					pDataType, checkAccess)\
		_getODentry(OD, wIndex, bSubindex, pDestData, pExpectedSize, \
					pDataType, checkAccess, 1)

#endif
