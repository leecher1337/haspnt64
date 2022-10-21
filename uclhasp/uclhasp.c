/*  UCLHasp Emulator for plugin HASPVDD driver
 *
 *  (c) leecher@dose.0wnz.at 2022
 *
 *  This module implements an interface with the well-known UCLHASP HASP-
 *  emulator, in order to create a userspace-only HASP emulation facility
 *  which is targeted at DOS programs running in NTVDM
 *
 */
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "registry.h"

VOID __stdcall Buffer0x28(VOID);
VOID __stdcall Buffer0x48(VOID);
VOID __stdcall Buffer0x54(VOID);


/* Main dispatch routine for the call emulation
 *
 * Parameters:
 *  Buffer    - [IN ] Buffer to process, gets read and filled by underlying
 *                    UCLhasp function
 *  Length    - [IN ] Length of Buffer
 *  ReadBytes - [OUT] Number of bytes read 
 * Returns:
 *  TRUE      - Operation successful
 *  FALSE     - Failed
 */
__declspec(dllexport) BOOL __stdcall
CallHardlock(HaspBufferInStruc* HaspBuffer, ULONG HaspBufferLength, PDWORD ReadBytes)
{
	switch (HaspBufferLength) {
		case 0x48:
		{
			__asm
			{
				push	ebp
				mov	ebp, HaspBuffer
				call	Buffer0x48
				pop	ebp
			}
			break;
		}
		case 0x54:
		{
			__asm {
				push	ebp
				mov	ebp, HaspBuffer
				call	Buffer0x54
				pop	ebp
			}

			break;
		}
		case 0x28:
		{
			__asm
			{
				push	ebp
				mov	ebp, HaspBuffer
				call	Buffer0x28
				pop	ebp
			};
			break;
		}
		default:
		{
			OutputDebugStringA("Buffer Length is not supported");
			return FALSE;
		}
	}
	if (ReadBytes) *ReadBytes = HaspBufferLength;
	return TRUE;
}

/* Loads dongle dumps from potential locations of well-known Dongle Emulators
 *
 * Returns:
 *  TRUE      - Some dumps were found and loaded
 *  FALSE     - No dumps were found
 */
BOOL LoadDumps(void)
{
	t_Supported = 0;
	heapSupported = NULL;
	heapMemory = NULL;

	LoadDumpKey("SYSTEM\\CurrentControlSet\\Services\\Emulator\\Hasp\\Dump");
	LoadDumpKey("SYSTEM\\CurrentControlSet\\MultiKey\\Dumps");
	LoadDumpKey("SYSTEM\\CurrentControlSet\\Mkbus");
	if (!t_Supported) return FALSE;
	return TRUE;
}


/* DLL entry point */
#ifdef _DEBUG
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
#else
BOOL WINAPI _DllMainCRTStartup(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
#endif
{
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(hinstDLL);
		if (!LoadDumps())
		{
			MessageBoxA(NULL, "No dumps of dongle keys found!\n", "UCLHASP Error", MB_OK | MB_ICONEXCLAMATION);
			return FALSE;
		}
		break;
	case DLL_PROCESS_DETACH:
		FreeDumpMemory();
		break;
	}
	return TRUE;
}
