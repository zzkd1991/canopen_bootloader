#include "my_canopen_adapter.h"
#include "objdictdef.h"
#include "data.h"
#include "sdo.h"
#include "objacces.h"
#include "applicfg.h"


#define getSDOcs(byte)	(byte >> 5)

#define getSDOn2(byte)	((byte >> 2) & 3)

#define getSDOn3(byte)	((byte >> 1) & 7)

#define getSDOe(byte)	((byte >> 1) & 1)

#define getSDOs(byte)	(byte & 1)

#define getSDOc(byte)	(byte & 1)

#define getSDOt(byte)	((byte >> 4) & 1)

#define getSDOindex(byte1, byte2)	(((UNS16)byte1 << 8) | ((UNS16)byte2))

#define getSDOsubIndex(byte3)	(byte3)

UNS8 sendSDO(CO_Data *d, UNS8 whoami, UNS8 CliServNbr, UNS8 *pData);


UNS8 proceedSDO(CO_Data *d, Message *m)
{
	UNS8 err;
	UNS8 cs;
	UNS8 line;
	UNS32 nbBytes;
	UNS8 CliServNbr;
	UNS8 whoami = SDO_UNKNOWN;
	UNS32 errorCode;
	UNS8 data[8];
	UNS16 index;
	UNS8 subIndex;
	UNS32 i;
	UNS8 j;
	UNS32 *pCobId = NULL;
	UNS16 offset;
	UNS16 lastIndex;

	extern Message mym;
	memcpy(&mym, m, sizeof(*m));
	
	offset = d->firstIndex->SDO_SVR;
	lastIndex = d->lastIndex->SDO_SVR;
	j = 0;
	if(offset) while(offset <= lastIndex)
	{
		if(d->objdict[offset].bSubCount <= 1)
		{
			return 0xFF;
		}
		
		//look for the cobid received.
		pCobId = (UNS32*)d->objdict[offset].pSubindex[1].pObject;
		if(*pCobId == UNS16_LE(m->cob_id))
		{
			whoami = SDO_SERVER;
			
			CliServNbr = j;
			break;
		}
		j++;
		offset++;
	}
	
	if(whoami == SDO_UNKNOWN)
	{
		return 0xFF;
	}
	
	if((*m).len != 8)
	{
		return 0xFF;
	}
	
	cs = getSDOcs(m->data[0]);

	err = getSDOlineOnUse(d, CliServNbr, whoami, &line);
	
	if(cs == 1)
	{
		if(whoami == SDO_SERVER)
		{
			index = getSDOindex(m->data[1], m->data[2]);
			subIndex = getSDOsubIndex(m->data[3]);


			err = getSDOfreeLine(d, whoami, &line);
			if(err)
			{
				failedSDO(d, line, CliServNbr, index, subIndex, SDOABT_LOCAL_CTRL_ERROR);
				return 0xFF;
			}

			initSDOline(d, line, CliServNbr, index, subIndex, SDO_DOWNLOAD_IN_PROGRESS);
			
			if(getSDOe(m->data[0]))
			{
				//nb of data to be downloaded
				nbBytes = 4 - getSDOn2(m->data[0]);
				//storing the data in the line structure
				d->transfers[line].count = nbBytes;
				err = SDOtoLine(d, line, nbBytes, (*m).data + 4);
				
				if(err)
				{
					failedSDO(d, CliServNbr, whoami, index, subIndex, SDOABT_GENERAL_ERROR);
					return 0xFF;
				}
				
				errorCode = SDOlineToObjdict(d, line);
				if(errorCode)
				{
					failedSDO(d, CliServNbr, whoami, index, subIndex, errorCode);
					return 0xFF;
				}
				
				//release of the line
				resetSDOline(d, line);
				
				/* Sending a SDO, cs = 3*/
				data[0] = 3 << 5;
				data[1] = index & 0xFF;
				data[2] = (index >> 8) & 0xFF;
				data[3] = subIndex;
				for(i = 4; i < 8; i++)
					data[i] = 0;
				
				sendSDO(d, whoami, CliServNbr, data);
			}
		}
		
	}
	else if(cs == 2)
	{
		if(whoami == SDO_SERVER)
		{
			index = getSDOindex(m->data[1], m->data[2]);
			subIndex = getSDOsubIndex(m->data[3]);
			if(!err)
			{
				printf("SDO error : Transmission yet started at line : %d", line);
				printf("Server Nbr = %d", CliServNbr);
				failedSDO(d, CliServNbr, whoami, index, subIndex, SDOABT_LOCAL_CTRL_ERROR);
				return 0xFF;
			}
			//No line on use. Great !
			// Try to open a new line.
			err = getSDOfreeLine(d, whoami, &line);
			if(err)
			{
				printf("SDO error : No line free, too many SDO in progress, Aborted.");
				failedSDO(d, CliServNbr, whoami, index, subIndex, SDOABT_LOCAL_CTRL_ERROR);
				return 0xFF;
			}

			initSDOline(d, line, CliServNbr, index, subIndex, SDO_UPLOAD_IN_PROGRESS);

			errorCode = objdictToSDOline(d, line);

			if(errorCode)
			{
				printf("SDO error : Unable to copy the data from object dictinoary. Err code : %ld", errorCode);
				failedSDO(d, CliServNbr, whoami, index, subIndex, errorCode);
				return 0xFF;
			}

			getSDOlineRestBytes(d, line, &nbBytes);
			if(nbBytes > 4)
			{
				data[0] = (2 << 5) | 1;
				data[1] = index & 0xFF;
				data[2] = (index >> 8) & 0xFF;
				data[3] = subIndex;
				data[4] = (UNS8)nbBytes;
				data[5] = (UNS8)(nbBytes >> 8);
				data[6] = (UNS8)(nbBytes >> 16);
				data[7] = (UNS8)(nbBytes >> 24);
				sendSDO(d, whoami, CliServNbr, data);
			}
			else
			{
				data[0] = (UNS8)((2 << 5) | ((4 - nbBytes) << 2) | 3);
				data[1] = index & 0xFF;
				data[2] = (index >> 8) & 0xFF;
				data[3] = subIndex;
				err = lineToSDO(d, line, nbBytes, data + 4);

				if(err)
				{
					failedSDO(d, CliServNbr, whoami, index, subIndex, SDOABT_GENERAL_ERROR);
					return 0xFF;
				}
				for(i = 4 + nbBytes; i < 8; i++)
					data[i] = 0;

				sendSDO(d, whoami, CliServNbr, data);
				resetSDOline(d, line);
			}
		}
	}

	return 0;
}

UNS8 initSDOline(CO_Data *d, UNS8 line, UNS8 CliServNbr, UNS16 index, UNS8 subIndex, UNS8 state)
{
	d->transfers[line].CliServNbr = CliServNbr;
	d->transfers[line].index = index;
	d->transfers[line].subIndex = subIndex;
	d->transfers[line].state = state;
	d->transfers[line].toggle = 0;
	d->transfers[line].count = 0;
	d->transfers[line].offset = 0;
	d->transfers[line].dataType = 0;
	d->transfers[line].Callback = NULL;
	
	return 0;
}

UNS8 SDOtoLine(CO_Data *d, UNS8 line, UNS32 nbBytes, UNS8 *data)
{
	UNS8 i;
	UNS32 offset;
	
	offset = d->transfers[line].offset;
	
	for(i = 0; i < nbBytes; i++)
		d->transfers[line].data[offset + i] = *(data + i);
	
	d->transfers[line].offset = d->transfers[line].offset + nbBytes;
	
	return 0;
}

UNS8 sendSDOabort(CO_Data *d, UNS8 whoami, UNS8 CliServNbr, UNS16 index, UNS8 subIndex, UNS32 abortCode)
{
	UNS8 data[8];
	UNS8 ret;
	
	data[0] = 0x80;
	//Index
	data[1] = index & 0xFF;
	data[2] = (index >> 8) & 0xFF;
	//SubIndex
	data[3] = subIndex;
	//Data
	data[4] = (UNS8)(abortCode & 0xFF);
	data[5] = (UNS8)((abortCode >> 8) & 0xFF);
	data[6] = (UNS8)((abortCode >> 16) & 0xFF);
	data[7] = (UNS8)((abortCode >> 24) & 0xFF);
	ret = sendSDO(d, whoami, CliServNbr, data);
	
	return ret;
}

UNS8 sendSDO(CO_Data *d, UNS8 whoami, UNS8 CliServNbr, UNS8 *pData)
{
	UNS16 offset;
	UNS8 i;
	Message m;
	
	if(whoami == SDO_SERVER)
	{
		offset = d->firstIndex->SDO_SVR;
		if((offset == 0) || (offset + CliServNbr) > d->lastIndex->SDO_SVR)
		{
			return 0xFF;
		}
		
		m.cob_id = (UNS16) *((UNS32 *)d->objdict[offset + CliServNbr].pSubindex[2].pObject);
	}
	else/* case client*/
	{
		offset = d->firstIndex->SDO_CLT;
		if((offset == 0) || ((offset + CliServNbr) > d->lastIndex->SDO_CLT))
		{
			return 0xFF;
		}
		
		m.cob_id = (UNS16) *((UNS32*)d->objdict[offset + CliServNbr].pSubindex[1].pObject);
	}
	
	m.rtr = NOT_A_REQUEST;
	m.len = 8;
	for(i = 0; i < 8; i++)
	{
		m.data[i] = pData[i];
	}
	
	return Can_Send(d->canHandle, &m);
}

UNS32 SDOlineToObjdict(CO_Data * d, UNS8 line)
{
	UNS32 size;
	UNS32 errorCode;

	if(d->transfers[line].count == 0)
		d->transfers[line].count = d->transfers[line].offset;

	size = d->transfers[line].count;

	errorCode = setODentry(d, d->transfers[line].index, d->transfers[line].subIndex, (void*)d->transfers[line].data, &size, 1);

	if(errorCode != OD_SUCCESSFUL)
		return errorCode;

	return 0;
}

void resetSDOline(CO_Data * d, UNS8 line)
{
	UNS32 i;

	initSDOline(d, line, 0, 0, 0, SDO_RESET);
	for(i = 0; i < SDO_MAX_LENGTH_TRANSFER; i++)
		d->transfers[line].data[i] = 0;

	d->transfers[line].whoami = 0;
	d->transfers[line].abortCode = 0;
}

UNS8 failedSDO(CO_Data * d, UNS8 CliServNbr, UNS8 whoami, UNS16 index, UNS8 subIndex, UNS32 abortCode)
{
	UNS8 err;
	UNS8 line;
	err = getSDOlineOnUse(d, CliServNbr, whoami, &line);
	if(!err)
		printf("FailedSDO : line found %d: ", line);

	if((!err) && (whoami == SDO_SERVER))
	{
		resetSDOline(d, line);
		printf("FailedSDO : line released %d: ", line);
	}

	if((!err) && (whoami == SDO_CLIENT))
	{
		d->transfers[line].abortCode = abortCode;
	}

	err = sendSDOabort(d, whoami, CliServNbr, index, subIndex, abortCode);
	if(err)
	{
		return 0xFF;
	}

	return 0;
}

UNS8 getSDOlineOnUse(CO_Data * d, UNS8 CliServNbr, UNS8 whoami, UNS8 * line)
{
	UNS8 i;

	for(i = 0; i < SDO_MAX_SIMULTANEOUS_TRANSFERS; i++)
	{
		if((d->transfers[i].state != SDO_RESET) && 
			(d->transfers[i].state != SDO_ABORTED_INTERNAL) &&
			(d->transfers[i].CliServNbr == CliServNbr) &&
			(d->transfers[i].whoami == whoami))
		{
			if(line)
				*line = i;
			return 0;
		}
	}

	return 0xFF;
}


UNS8 getSDOfreeLine(CO_Data *d, UNS8 whoami, UNS8 *line)
{
	UNS8 i;

	for(i = 0; i < SDO_MAX_SIMULTANEOUS_TRANSFERS; i++)
	{
		if(d->transfers[i].state == SDO_RESET)
		{
			*line = i;
			d->transfers[i].whoami = whoami;
			return 0;
		}
	}

	printf("Too many SDO in progress. Aborted.");
	return 0xFF;
}

UNS32 objdictToSDOline(CO_Data *d, UNS8 line)
{
	UNS32 size = SDO_MAX_LENGTH_TRANSFER;
	UNS8 dataType;
	UNS32 errorCode;

	errorCode = getODentry(d, d->transfers[line].index, d->transfers[line].subIndex, (void*)d->transfers[line].data, &size, &dataType, 1);

	if(errorCode != OD_SUCCESSFUL)
		return errorCode;

	d->transfers[line].count = size;
	d->transfers[line].offset = 0;

	return 0;
}

UNS8 getSDOlineRestBytes(CO_Data *d, UNS8 line, UNS32 *nbBytes)
{
	if(d->transfers[line].count == 0)
		*nbBytes = 0;
	else
		*nbBytes = d->transfers[line].count - d->transfers[line].offset;

	return 0;
}

UNS8 lineToSDO(CO_Data * d, UNS8 line, UNS32 nbBytes, UNS8 * data)
{
	UNS8 i;
	UNS32 offset;

	if((d->transfers[line].offset + nbBytes) > d->transfers[line].count)
	{
		return 0xFF;
	}

	offset = d->transfers[line].offset;

	for(i = 0; i < nbBytes; i++)
		*(data + i) = d->transfers[line].data[offset + i];

	d->transfers[line].offset = d->transfers[line].offset + nbBytes;

	return 0;
}

