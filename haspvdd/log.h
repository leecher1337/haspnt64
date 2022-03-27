#pragma once

#define LOG_HEADER "________ [Ln] [ Ticks   ] [Service  ] [Param1/AX] [Param2/BX] [Param3/CX] [Param4/DX] [Para1R/DI] [Para2R/SI] [Para3R/ES] [Para4R/AX]\n"
#define LOGTYPE_STR "OUT", "IN ", "DAT" 

#pragma pack(1)
typedef struct
{
	SYSTEMTIME st;
	BYTE Direction;
	USHORT Length;
} LOG_HDR;
#pragma pack()

void DbgPrintHeader(void);
void DbgDumpMsg(BYTE Direction, PBYTE Data, SIZE_T Length);
void LogInit(HKEY hKey);
void LogExit(void);
void LogError(char* Message);
void LogLastError(char* Message);



