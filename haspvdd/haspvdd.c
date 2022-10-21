/*  HASPVDD Driver 
 *
 *  (c) leecher@dose.0wnz.at 2022
 *
 *  This module implements the main dispatcher for the VDD.
 *
 */
#include "haspvdd.h"
#include <winioctl.h>
#include "hasp.h"
#include "haspio.h"
#include "haspcrypt.h"
#include "hardlockdrv.h"
#include "log.h"
#include "emulate.h"
#define i386
#include <vddsvc.h>
#pragma comment (lib, "ntvdm.lib")

static DWORD m_InterfaceMode = MODE_IOCTLIF;
static fnHaspIOCtl pHaspIOCtl = HaspIOCtl;

/* DLL entry point, gets called on load and unload, initializes and cleans up
 * module internal structures
 *
 * Parameters:
 *  hVdd       - Handle of the module; this is not used.
 *  dwReason   - DLL_PROCESS_ATTACH if the DLL is loaded
 *               DLL_PROCESS_DETACH if it is unloaded.
 *  lpReserved - Not used.
 * Returns:
 *  TRUE  - Success
 *  FALSE - Failure
 */
__declspec(dllexport) BOOL __cdecl
VDDInitialize(
	HANDLE   hVdd,
	DWORD    dwReason,
	LPVOID   lpReserved)
{
	if (dwReason == DLL_PROCESS_DETACH)
	{
		CloseHardlock();
		LogExit();
	}

	return TRUE;
}

/* RegisterModule processing.
 * This function is called when the DOS portion calls the REGISTERMODULE 
 * function. Loads configuration.
 *
 * Returns:
 *  set Carry flag to 0 on success
 */
__declspec(dllexport) VOID __cdecl
VDDRegisterInit(
	VOID
)
{
	HKEY hKey;
	DWORD cbData;

	if (RegOpenKey(HKEY_LOCAL_MACHINE, "System\\CurrentControlSet\\Services\\HaspNt\\Parameters", &hKey) == ERROR_SUCCESS)
	{
		LogInit(hKey);
		
		cbData = sizeof(m_InterfaceMode);
		RegQueryValueEx(hKey, "LegacyVDDInterface", NULL, NULL, (LPBYTE)&m_InterfaceMode, &cbData);

		if (m_InterfaceMode == MODE_LEGACYIF) pHaspIOCtl = CallLegacyHardlock;
		EmulatorInit(hKey, &m_InterfaceMode, &pHaspIOCtl);

		RegCloseKey(hKey);
	}

	DbgPrintHeader();
	setCF(0);
}

/* DispatchCall processing
 * This function is called when the DOS portion calls the DISPATCHCALL 
 * function. Processes the incoming DOS buffer.
 *
 * Returns:
 *  set Carry flag to 0 on success, 1 on failure
 */
__declspec(dllexport) VOID __cdecl
VDDDispatch()
{
	DWORD ReadBytes;
	BOOL Success = FALSE;
	SIZE_T LockedMemSize = 0;
	PVOID LockedMemPtr = NULL;
	HaspBufferInStruc *IOBuffer = (HaspBufferInStruc * )GetVDMPointer(getDI() | (getES() << 16), getCX(), 0);

	Decrypt28((PUSHORT)IOBuffer, sizeof(HaspBufferInStruc));
	DbgDumpMsg(0, (PBYTE)IOBuffer, sizeof(HaspBufferInStruc));

	IOBuffer->DOSBuffer.Service2 = 1;
	switch (IOBuffer->DOSBuffer.Service)
	{
	case MEMOHASP_READBLOCK:
	case MEMOHASP_WRITEBLOCK:
	case TIMEHASP_READBLOCK:
	case TIMEHASP_WRITEBLOCK:
	case LOCALHASP_ENCODEDATA:			// Original VDD: 0x37, but that code is invalid??
	case LOCALHASP_DECODEDATA:			// Original VDD: 0x38, but that code is invalid??
		IOBuffer->DOSBuffer.AX = (DWORD)GetVDMPointer(
			IOBuffer->DOSBuffer.AX | (IOBuffer->DOSBuffer.ES << 16),
			IOBuffer->DOSBuffer.SI,
			HIBYTE(IOBuffer->DOSBuffer.Param1) == 1);
		if (m_InterfaceMode == MODE_LEGACYIF)
		{
			LockedMemPtr = (PVOID)IOBuffer->DOSBuffer.AX;
			LockedMemSize = IOBuffer->DOSBuffer.SI;
			if (IOBuffer->DOSBuffer.Service != TIMEHASP_READBLOCK &&
				IOBuffer->DOSBuffer.Service != TIMEHASP_WRITEBLOCK)
				LockedMemSize *= 2;
			DbgDumpMsg(2, LockedMemPtr, LockedMemSize);
			if (!VirtualLock(LockedMemPtr, LockedMemSize))
				LockedMemSize = 0;
		}
		break;
	}

	if (m_InterfaceMode == MODE_LEGACYIF)
	{
		HaspBufferInStruc Buf;

		Encrypt28((PUSHORT)IOBuffer, sizeof(HaspBufferInStruc));
		Buf = *IOBuffer;
		Success = pHaspIOCtl(&Buf, sizeof(HaspBufferInStruc), &ReadBytes);
		*IOBuffer = Buf;
		Decrypt28((PUSHORT)&Buf, sizeof(HaspBufferInStruc));
		if (LockedMemSize)
		{
			DbgDumpMsg(2, LockedMemPtr, LockedMemSize);
			VirtualUnlock(LockedMemPtr, LockedMemSize);
		}
		DbgDumpMsg(1, (PBYTE)&Buf, sizeof(HaspBufferInStruc));
	}
	else
	{
		Success = pHaspIOCtl(IOBuffer, sizeof(HaspBufferInStruc), &ReadBytes);
		DbgDumpMsg(1, (PBYTE)IOBuffer, sizeof(HaspBufferInStruc));
	}

	if (Success)
	{
		setCF(0);
		setCX((USHORT)ReadBytes);
	}
	else
	{
		setCF(1);
	}
}

