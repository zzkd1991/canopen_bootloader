#include "my_canopen_adapter.h"
#include "objdictdef.h"
#include "data.h"
#include "sdo.h"


UNS8 slaveNode = 0x0a;

void _storeODSubIndex(CO_Data * d, UNS16 wIndex, UNS8 bSubindex, void * ptr, UNS32 size)
{

}

void setNodeId(CO_Data *d, UNS8 nodeId)
{
	UNS16 offset = d->firstIndex->SDO_SVR;

	if(!(nodeId > 0 && nodeId <= 127))
	{
		printf("Invalid NodeID %d\n", nodeId);
		return;
	}

	if(offset)
	{
		//Adjust COB-ID Client->Server (rx) only id alread set to default value or id not valid (id == 0xFF)
		if((*(UNS32 *)d->objdict[offset].pSubindex[1].pObject == 0x600 + *d->bDeviceNodeId) || (*d->bDeviceNodeId == 0xFF))
		{
			*(UNS32*)d->objdict[offset].pSubindex[1].pObject = 0x600 + nodeId;
		}

		//Adjust COB-ID Server -> Client (tx) only  id already set to default value or id not valid (id == 0xFF)
		if((*(UNS32*)d->objdict[offset].pSubindex[2].pObject == 0x580 + *d->bDeviceNodeId) || (*d->bDeviceNodeId == 0xFF))
		{
			*(UNS32*)d->objdict[offset].pSubindex[2].pObject = 0x580 + nodeId;
		}


		*d->bDeviceNodeId = nodeId;
	}
}


void InitNode(CO_Data *d ,UNS32 id)
{
	extern CO_Data ObjDict_Data;	
	setNodeId(&ObjDict_Data, slaveNode);

}

int InitCANdevice(UNS32 baudrate, UNS8 node)
{
	slaveNode = node;

	return 0;
}

