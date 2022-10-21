//----------------------------------------------------------------------
//
// support address meteo@null.net
//
//----------------------------------------------------------------------
//----------------------------------------------------------------------
// HASP emulator for Windows NT 4.0, (x)1998-99 by MeteO, Fixit
// Private property of //UCL
//----------------------------------------------------------------------
// HASP dump defines
//
// Created: 28.05.97 23:56
//
//----------------------------------------------------------------------
#ifndef _HASP_H
#define _HASP_H
//
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
//
#define	MEMO_BUFFER_SIZE	56
//

#define HASP3			0
#define MemoHASP		1
#define	TimeHASP		2
#define NetHASP			4
 //
// HASP basical services
//
#define	HASP_INSTALLED		1   // 01h
#define	HASP_CODE		2   // 02h
#define	READ_MEMO               3   // 03h
#define WRITE_MEMO              4   // 04h
#define GET_HASP_STATUS         5   // 05h
#define	GET_ID_NUM		6   // 06h
//
#define	UNDOC_0Bh		11  // 0Bh
#define	UNDOC_0Ch		12  // 0Ch
//
#define	MEMOHASP_READ_BLOCK	50  // 32h
#define	MEMOHASP_WRITE_BLOCK	51  // 33h
//
// A list of NetHASP services.
//
#define	NET_LAST_STATUS		40  // 28h
#define	NET_GET_HASP_CODE	41  // 29h
#define	NET_LOGIN		42  // 2Ah
#define	NET_LOGOUT		43  // 2Bh
#define	NET_READ_WORD		44  // 2Ch
#define	NET_WRITE_WORD		45  // 2Dh
#define	NET_GET_ID_NUMBER	46  // 2Eh
#define	NET_READ_MEMO_BLOCK	52  // 34h
#define	NET_WRITE_MEMO_BLOCK    53  // 35h
#define	SET_SERVER_BY_NAME	96  // 60h
#define	SET_CONFIG_FILENAME	55h // 55h
//
// A list of TimeHASP services.
//
#define	TIME_SET_TIME		70  // 46h
#define	TIME_GET_TIME		71  // 47h
#define	TIME_SET_DATE		72  // 48h
#define	TIME_GET_DATE		73  // 49h
#define	TIME_WRITE_MEMORY	74  // 4Ah
#define	TIME_READ_MEMORY	75  // 4Bh
#define	TIME_WRITE_MEMORY_BLOCK	76  // 4Ch
#define	TIME_READ_MEMORY_BLOCK	77  // 4Dh
#define	TIME_GET_ID_NUM     	78  // 4Eh
//
//
#define	OK			0   //
#define	NET_READ_ERROR		131 // 83h
#define	NET_WRITE_ERROR		132 // 84h
        //
#define	OTHER_BLOCK_OP_MIN	81  // 51h
#define	OTHER_BLOCK_OP_MAX	99  // 63h
//
//
// HASP block services. (compatibility list)
//
#define	MEMOHASP_READ_BLOCK	50
#define	MEMOHASP_WRITE_BLOCK	51
#define	NETHASP_READ_BLOCK	52
#define	NETHASP_WRITE_BLOCK	53
#define	TIMEHASP_WRITE_BLOCK	76
#define	TIMEHASP_READ_BLOCK	77

//

typedef struct{
ULONG	Reserved01[5];
USHORT	__AX;
UCHAR	__BH;
UCHAR	Reserved06;
ULONG	__EAX;
UCHAR	__BL;
UCHAR	Reserved07[3];
ULONG	__ECX;
ULONG	__EDX;
ULONG	__EDI;
ULONG	__ESI;
ULONG	Reserved0A[4];
ULONG	__BufOffs;
}HaspBufferInStruc;

typedef struct{
ULONG	o_Reserved01[10];
ULONG	o_EAX;
ULONG	o_EBX;
ULONG	o_ECX;
ULONG	o_EDX;
}HaspBufferOutStruc;

typedef struct{
ULONG	v_Reserved01[6];
ULONG	v_EAX;
ULONG	v_EBX;
ULONG	v_ECX;
ULONG	v_EDX;
}HaspBufferOutStrucV86;

typedef struct{
ULONG	Pwd1;
ULONG	Pwd2;
PSHORT  PMemory;	
ULONG	Type;
}Keys;

#endif //_HASP_H
