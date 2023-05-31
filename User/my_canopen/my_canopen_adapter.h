#ifndef __MY_CANOPEN_ADAPTER_H__
#define __MY_CANOPEN_ADAPTER_H__

#include "applicfg.h"
#include "objdictdef.h"
/* Status of the node during the SDO transfer : */
#define SDO_SERVER  0x1
#define SDO_CLIENT  0x2
#define SDO_UNKNOWN 0x3


#ifdef CANOPEN_BIG_ENDIAN

/* Warning: the argument must not update pointers, e.g. *p++ */

#define UNS16_LE(v)  ((((UNS16)(v) & 0xff00) >> 8) | \
		      (((UNS16)(v) & 0x00ff) << 8))

#define UNS32_LE(v)  ((((UNS32)(v) & 0xff000000) >> 24) |	\
		      (((UNS32)(v) & 0x00ff0000) >> 8)  |	\
		      (((UNS32)(v) & 0x0000ff00) << 8)  |	\
		      (((UNS32)(v) & 0x000000ff) << 24))

#else

#define UNS16_LE(v)  (v)

#define UNS32_LE(v)  (v)

#endif

/** definitions used for object dictionary access. ie SDO Abort codes . (See DS 301 v.4.02 p.48)
 */
#define OD_SUCCESSFUL 	             0x00000000
#define OD_READ_NOT_ALLOWED          0x06010001
#define OD_WRITE_NOT_ALLOWED         0x06010002
#define OD_NO_SUCH_OBJECT            0x06020000
#define OD_NOT_MAPPABLE              0x06040041
#define OD_LENGTH_DATA_INVALID       0x06070010
#define OD_NO_SUCH_SUBINDEX 	     0x06090011
#define OD_VALUE_RANGE_EXCEEDED      0x06090030 /* Value range test result */
#define OD_VALUE_TOO_LOW             0x06090031 /* Value range test result */
#define OD_VALUE_TOO_HIGH            0x06090032 /* Value range test result */
/* Others SDO abort codes */
#define SDOABT_TOGGLE_NOT_ALTERNED   0x05030000
#define SDOABT_TIMED_OUT             0x05040000
#define SDOABT_OUT_OF_MEMORY         0x05040005 /* Size data exceed SDO_MAX_LENGTH_TRANSFER */
#define SDOABT_GENERAL_ERROR         0x08000000 /* Error size of SDO message */
#define SDOABT_LOCAL_CTRL_ERROR      0x08000021 

#define SDO_MAX_LENGTH_TRANSFER 50


#define REPEAT_SDO_MAX_SIMULTANEOUS_TRANSFERS_TIMES(repeat)\
repeat
#define RW	0x00
#define WO	0x01
#define RO	0x02

#define REQUEST	1
#define NOT_A_REQUEST 0

#define SDO_RESET	0x0
#define SDO_FINISHED	0x01
#define SDO_ABORTED_RCV	0x80
#define SDO_ABORTED_INTERNAL	0x85
#define SDO_DOWNLOAD_IN_PROGRESS	0x2
#define SDO_UPLOAD_IN_PROGRESS	0x3

/* Others SDO abort codes 
 */
#define SDOABT_TOGGLE_NOT_ALTERNED   0x05030000
#define SDOABT_TIMED_OUT             0x05040000
#define SDOABT_OUT_OF_MEMORY         0x05040005 /* Size data exceed SDO_MAX_LENGTH_TRANSFER */
#define SDOABT_GENERAL_ERROR         0x08000000 /* Error size of SDO message */
#define SDOABT_LOCAL_CTRL_ERROR      0x08000021 


//typedef struct struct_CO_Data CO_Data;
#define NODE_SLAVE 	0x0d

/* Prototypes of function provided by object dictionnary */
UNS32 ObjDict_valueRangeTest (UNS8 typeValue, void * value);
const indextable * ObjDict_scanIndexOD (UNS16 wIndex, UNS32 * errorCode, ODCallback_t **callbacks);
int InitCANdevice(UNS32 baudrate, UNS8 node);
void InitNode(CO_Data *d ,UNS32 id);



#endif
