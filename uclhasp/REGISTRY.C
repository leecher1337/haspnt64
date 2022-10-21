#include "registry.h"
#include "DEBUG.H"

#undef RtlMoveMemory
EXTERN_C void _declspec(dllimport) WINAPI RtlMoveMemory(PVOID, const VOID*, SIZE_T);


PCHAR MemoryValue;
PCHAR CurrentDump;

#define ADD_ALLOCATED_DUMP	0x10
#define MEMORY_MAX_LENGTH	0x200

// Poor man's strtol (to get rid of CRT):
static unsigned char nibbleFromChar(char c)
{
	if (c >= '0' && c <= '9') return c - '0';
	if (c >= 'a' && c <= 'f') return c - 'a' + 10;
	if (c >= 'A' && c <= 'F') return c - 'A' + 10;
	return 255;
}

/* Converts hex string to a DWORD
 *
 * Parameters:
 *  inhex     - [IN ] Hex String to convert without 0x prefix
 *  outdw     - [OUT] DWORD to be filled with converted string
 */
void hexStringToDWORD(char* inhex, DWORD *outdw)
{
	unsigned char* p;
	int i;

	for (i = sizeof(DWORD)-1, p = (unsigned char*)inhex; i >= 0; i--) {
		((unsigned char*)outdw)[i] = (nibbleFromChar(*p) << 4) | nibbleFromChar(*(p + 1));
		p += 2;
	}
}

/* Allocate/reallocate memory block on heap to desired size
 * ~= realloc without CRT
 *
 * Parameters:
 *  ppMem     - [IN ] Pointer to a pointer to memory block to (re)allocate
 *  dwBytes   - [IN ] Desired Number of bytes for memory block
 * Returns:
 *  TRUE      - Operation successful
 *  FALSE     - Failed
 */
static BOOL allocate(PVOID* ppMem, SIZE_T dwBytes)
{
	PVOID pMem;

	if (!*ppMem)
	{
		if (*ppMem = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwBytes)) return TRUE;
	}
	else if (pMem = HeapReAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, *ppMem, dwBytes))
	{
		*ppMem = pMem;
		return TRUE;
	}
	NTPrint("Cannot allocate memory, err=%d\n", GetLastError());
	return FALSE;
}


/* Looks in given registry path for Dongle Dumps and adds them to internal
 * list to be checked during emulation
 *
 * Parameters:
 *  pszKey    - [IN ] Registry key to check
 * 
 * Returns:
 *  TRUE      - Successfully scanned registry key
 *  FALSE     - Failed
 */
BOOL LoadDumpKey(char* pszKey)
{
	DWORD i, cbData;
	char szKeyName[32], szKey[512];
	DWORD cchName = sizeof(szKeyName), cSubKeys;
	HKEY hKey, hKeyDump;

	if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, pszKey, 0, KEY_READ, &hKey) == ERROR_SUCCESS)
	{
		if (RegQueryInfoKeyA(hKey, NULL, NULL, NULL, &cSubKeys, NULL, NULL, NULL, NULL,
			NULL, NULL, NULL) != ERROR_SUCCESS)
		{
			NTPrint("Cannot enumerate keys under %s: err=%d\n", pszKey, GetLastError());
			RegCloseKey(hKey);
			return FALSE;
		}

		if (!allocate(&heapSupported, sizeof(Keys) * (t_Supported + cSubKeys + ADD_ALLOCATED_DUMP)) ||
			!allocate(&heapMemory, MEMORY_MAX_LENGTH * (t_Supported + cSubKeys + ADD_ALLOCATED_DUMP)))
		{
			RegCloseKey(hKey);
			return FALSE;
		}

		for (i = 0; i < cSubKeys; i++)
		{
			if (RegEnumKeyExA(hKey, i, szKeyName, &cchName, NULL, NULL, NULL, NULL) != ERROR_SUCCESS)
				continue;
			wsprintfA(szKey,  "%s\\%s", pszKey, szKeyName);
			cchName = sizeof(szKeyName);
			if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, szKey, 0, KEY_READ, &hKeyDump) == ERROR_SUCCESS)
			{
				DWORD PW;
				ULONG Type;
				BYTE Data[MEMORY_MAX_LENGTH];

				// UCLHASP format: Pwd
				// Multikey format: could be PW
				cbData = sizeof(PW);
				if (!(RegQueryValueExA(hKeyDump, "Pwd", NULL, NULL, (LPBYTE)&PW, &cbData) == ERROR_SUCCESS ||
					RegQueryValueExA(hKeyDump, "PW", NULL, NULL, (LPBYTE)&PW, &cbData) == ERROR_SUCCESS))
				{
					hexStringToDWORD(szKeyName, &PW);
					if (PW == (DWORD)-1)
					{
						NTPrint("Key %s is not a valid key (Pwd missing)\n", szKeyName);
						RegCloseKey(hKeyDump);
						continue;
					}
				}
				heapSupported[t_Supported].Pwd1 = (ULONG) * ((PUSHORT)((PCHAR)&PW + 2));
				heapSupported[t_Supported].Pwd2 = (ULONG) * ((PUSHORT)((PCHAR)&PW + 0));

				// Multikey format: DongleType
				// UCLHASP format: Type
				cbData = sizeof(Type);
				if (!(RegQueryValueExA(hKeyDump, "DongleType", NULL, NULL, (LPBYTE)&Type, &cbData) == ERROR_SUCCESS ||
					RegQueryValueExA(hKeyDump, "Type", NULL, NULL, (LPBYTE)&Type, &cbData) == ERROR_SUCCESS))
				{
					NTPrint("Key %s is not a valid key (Has no DongleType specified)\n", szKeyName);
					RegCloseKey(hKeyDump);
					continue;
				}
				heapSupported[t_Supported].Type = Type;
				if (Type <= 4)
				{
					__asm {
						pushad
						mov eax, t_Supported
						inc eax
						call GetOffsetToMem
						mov  eax, heapMemory
						add  ebx, eax
						mov  MemoryValue, ebx
						popad
					}
				}
				heapSupported[t_Supported].PMemory = (PSHORT)MemoryValue;

				cbData = sizeof(ULONG);
				RegQueryValueExA(hKeyDump, "SN", NULL, NULL, (LPBYTE)heapSupported[t_Supported].PMemory, &cbData);
				cbData = sizeof(Data);
				if (RegQueryValueExA(hKeyDump, "Data", NULL, NULL, Data, &cbData) == ERROR_SUCCESS)
					RtlMoveMemory(((LPBYTE)heapSupported[t_Supported].PMemory) + sizeof(ULONG), Data, heapSupported[i].Type * 128 - 16);

				RegCloseKey(hKeyDump);
				t_Supported++;
			}
		}
		RegCloseKey(hKey);
	}
	return TRUE;
}
/* Hook to read registry on first dongle check.
 * However, as we do this on startup, we just always return success
 */
void __stdcall ReadRegistry(void)
{
	__asm stc;
}
/* Free dongle list loaded */
VOID FreeDumpMemory(VOID)
{
	HeapFree(GetProcessHeap(), 0, heapSupported);
	HeapFree(GetProcessHeap(), 0, heapMemory);
	return;
} //End of FreeDumpMemory