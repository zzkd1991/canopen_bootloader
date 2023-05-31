#include "my_canopen_adapter.h"
#include "objdictdef.h"
#include "data.h"
#include "sdo.h"
#include "applicfg.h"
#include "objacces.h"

uint8_t curr_addr = 0;

UNS8 accessDictionaryError(UNS16 index, UNS8 subIndex, UNS32 sizeDataDict, UNS32 sizeDataGiven, UNS32 code)
{
	printf("Dictinoary index %d: ", index);

	printf("	subindex %d: ", subIndex);

	switch(code)
	{
		case OD_NO_SUCH_OBJECT:
			printf("Index %d not found ", index);
			break;
		case OD_NO_SUCH_SUBINDEX:
			printf("SubIndex %d not found", subIndex);
			break;
		case OD_LENGTH_DATA_INVALID:
			printf("Conflict size data. Should be %ld (bytes) : ", sizeDataDict);
			printf("But you have given the size %ld: ",sizeDataGiven);
			break;
		case OD_NOT_MAPPABLE:
			printf("Not mappable data in a PDO at index	%d: ", index);
			break;
		case OD_VALUE_TOO_LOW:
			printf("Value range error : value too low. SDOabort %ld:  ", code);
			break;
		case OD_VALUE_TOO_HIGH:
			printf("Value range error : value too high. SDOabort %ld: ", code);
			break;
		default:
			printf("Unknown error code : %ld", code);
	}

	return 0;
}

UNS32 _getODentry(CO_Data *d, UNS16 wIndex, UNS8 bSubindex, void *pDestData, UNS32 *pExpectedSize, UNS8 *pDataType, UNS8 checkAccess, UNS8 endianize)
{
	UNS32 errorCode;
	UNS32 szData;

	const indextable *ptrTable;
	ODCallback_t *Callback;

	ptrTable = (*d->scanIndexOD)(wIndex, &errorCode, &Callback);

	if(errorCode != OD_SUCCESSFUL)
		return errorCode;
	if(ptrTable->bSubCount <= bSubindex)
	{
		accessDictionaryError(wIndex, bSubindex, 0, 0, OD_NO_SUCH_SUBINDEX);
		return OD_NO_SUCH_SUBINDEX;
	}

	if(pDestData == 0)
	{
		return SDOABT_GENERAL_ERROR;
	}

	if(ptrTable->pSubindex[bSubindex].size > *pExpectedSize)
	{
		*pExpectedSize = ptrTable->pSubindex[bSubindex].size;
		return SDOABT_OUT_OF_MEMORY;
	}

	*pDataType = ptrTable->pSubindex[bSubindex].bDataType;
	szData = ptrTable->pSubindex[bSubindex].size;

	memcpy(pDestData, ptrTable->pSubindex[bSubindex].pObject, szData);
	*pExpectedSize = szData;

	return OD_SUCCESSFUL;
}

UNS32 _setODentry(CO_Data *d, UNS16 wIndex, UNS8 bSubindex, void *pSourceData, UNS32 *pExpectedSize, UNS8 checkAccess, UNS8 endianize)
{
	UNS32 szData;
	UNS8 dataType;
	UNS32 errorCode;
	const indextable *ptrTable;
	ODCallback_t *Callback;

	ptrTable = (*d->scanIndexOD)(wIndex, &errorCode, &Callback);
	if(errorCode != OD_SUCCESSFUL)
		return errorCode;

	if(ptrTable->bSubCount <= bSubindex)
	{
		accessDictionaryError(wIndex, bSubindex, 0, *pExpectedSize, OD_NO_SUCH_SUBINDEX);
		return OD_NO_SUCH_SUBINDEX;
	}

	dataType = ptrTable->pSubindex[bSubindex].bDataType;
	szData = ptrTable->pSubindex[bSubindex].size;

	if(*pExpectedSize == 0 || *pExpectedSize == szData)
	{
		errorCode = (*d->valueRangeTest)(dataType, pSourceData);
		if(errorCode)
		{
			accessDictionaryError(wIndex, bSubindex, szData, *pExpectedSize, errorCode);
			return errorCode;
		}

		memcpy(ptrTable->pSubindex[bSubindex].pObject, pSourceData, *pExpectedSize);

		*pExpectedSize = szData;

		/*Callbacks*/
		if(Callback && Callback[bSubindex])
		{
			errorCode = (Callback[bSubindex])(d, ptrTable, bSubindex);
			if(errorCode != OD_SUCCESSFUL)
			{
				return errorCode;
			}
		}

		return OD_SUCCESSFUL;
	}
	else
	{
		*pExpectedSize = szData;
		accessDictionaryError(wIndex, bSubindex, szData, *pExpectedSize, OD_LENGTH_DATA_INVALID);
		return OD_LENGTH_DATA_INVALID;
	}
}

const indextable *scanIndexOD(CO_Data *d, UNS16 wIndex, UNS32 *errorCode, ODCallback_t **Callback)
{
	return (*d->scanIndexOD)(wIndex, errorCode, Callback);
}

UNS32 RegisterSetODentryCallBack(CO_Data* d, UNS16 wIndex, UNS8 bSubindex, ODCallback_t Callback)
{
	UNS32 errorCode;
	ODCallback_t *CallbackList;
	const indextable *odentry;

	odentry = scanIndexOD(d, wIndex, &errorCode, &CallbackList);

	if(errorCode == OD_SUCCESSFUL && CallbackList && bSubindex < odentry->bSubCount)
		CallbackList[bSubindex] = Callback;

	return errorCode;
}

