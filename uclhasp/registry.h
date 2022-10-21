//----------------------------------------------------------------------
//
// support address meteo@null.net
//
//----------------------------------------------------------------------
#ifndef _HASP_REGISTRY_H
#define _HASP_REGISTRY_H

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "hasp.h"



extern Keys* heapSupported;
extern PCHAR heapMemory;
extern ULONG t_Supported;


VOID __stdcall GetOffsetToMem(VOID);
VOID FreeDumpMemory(VOID);
BOOL LoadDumpKey(char* pszKey);
#endif
