/*  HASPVDD Driver 
 *
 *  (c) leecher@dose.0wnz.at 2022
 *
 * The very, very stupid emulator for very, very studid applications that don't even
 * use a randomized seed and only call function 2. Just setup a template with the
 * 16 bytes from Param 1 to Param 4 that need to be the answer (just copy the bytes
 * from [Param1] to [Param4] from the trace reply)
 *
 */

#include "haspvdd.h"
#include "hasp.h"
#include "haspcrypt.h"
#include "log.h"

BOOL EmulateCalls(HaspBufferInStruc* Buffer, ULONG Length, PDWORD ReadBytes);

static HaspDOSBufferStruc EmulatorCodeTemplate;

/* Initializes the emulator by loading its configuration
 *
 * Parameters:
 *  hKey            - Registry key of the node containing the configuration settings
 *  pInterfaceMode  - Interface mode will be set to appropriate dispatcher function
 *  pfnHaspIOCtl    - IO Control dispatcher function will be set accordingly
 *
 * Returns:
 *  TRUE  - Initialization succeeded
 *  FALSE - Failure to read configuration 
 */
BOOL EmulatorInit(HKEY hKey, DWORD *pInterfaceMode, fnHaspIOCtl *pfnHaspIOCtl)
{
	DWORD cbData = 4 * sizeof(DWORD);
	char szEmulator[MAX_PATH];

	// User just wants to emulate 02 call with predefined data
	if (RegQueryValueEx(hKey, "EmulateParams", NULL, NULL,
		(LPBYTE)&EmulatorCodeTemplate.Param1Ret, &cbData) == ERROR_SUCCESS &&
		cbData == 4 * sizeof(DWORD))
	{
		*pfnHaspIOCtl = EmulateCalls;
		*pInterfaceMode = MODE_IOCTLIF;
		return TRUE;
	}

	// User registered an emulator library
	cbData = sizeof(szEmulator);
	if (RegQueryValueEx(hKey, "EmulatorDLL", NULL, NULL, szEmulator, &cbData) == ERROR_SUCCESS)
	{
		HMODULE hDLL = LoadLibrary(szEmulator);
		fnHaspIOCtl pfunc;

		if (!hDLL)
		{
			LogLastError("Cannot load specified emulator DLL: ");
			return FALSE;
		}

		if (!(pfunc = (fnHaspIOCtl)GetProcAddress(hDLL, "CallHardlock")))
		{
			LogLastError("Emulator DLL doesn't contain correct dispatcher function: ");
			FreeLibrary(hDLL);
			return FALSE;
		}
		*pfnHaspIOCtl = pfunc;
		*pInterfaceMode = MODE_LEGACYIF;
		return TRUE;
	}

	return FALSE;
}

/* Emulates a call to the HASP dongle, dispatches incoming calls.
 * Expects a decrypted buffer in HaspBufferInStruc
 * Encrypts the buffer after filling it so that it can be passed on
 * to the next driver.
 *
 * Parameters:
 *  Buffer    - [IN]  Decrypted Buffer from caller to check
 *  Length    - [IN]  Length of Buffer 
 *  ReadBytes - [OUT] Number of bytes read 
 *
 * Returns:
 *  TRUE      - Incoming packet has been processed successfully
 *  FALSE     - Packet could not be processed, onknown Service code
 *              Even on FALSE, return Buffer is being Re-encrypted
 */
BOOL EmulateCalls(HaspBufferInStruc* Buffer, ULONG Length, PDWORD ReadBytes)
{
	BOOL bRet = TRUE;
	
	switch (Buffer->DOSBuffer.Service)
	{
	case LOCALHASP_ISHASP:
		Buffer->DOSBuffer.Param1Ret = 1;
		Buffer->DOSBuffer.Param2Ret = 1;
		Buffer->DOSBuffer.Param3Ret = HASPERR_SUCCESS;
		break;
	case LOCALHASP_HASPCODE:
		Buffer->DOSBuffer.Param1Ret = EmulatorCodeTemplate.Param1Ret;
		Buffer->DOSBuffer.Param2Ret = EmulatorCodeTemplate.Param2Ret;
		Buffer->DOSBuffer.Param3Ret = EmulatorCodeTemplate.Param3Ret;
		Buffer->DOSBuffer.Param4Ret = EmulatorCodeTemplate.Param4Ret;
		break;
	default:
	    bRet = FALSE;
		break;
	}

	Buffer->DOSBuffer.Service2 = (BYTE)Buffer->DOSBuffer.Service;
	Buffer->DOSBuffer.Param1 = Buffer->DOSBuffer.Param1Ret;
	Buffer->DOSBuffer.Param2 = Buffer->DOSBuffer.Param2Ret;
	Buffer->DOSBuffer.Param3 = Buffer->DOSBuffer.Param3Ret;
	Buffer->DOSBuffer.Param4 = Buffer->DOSBuffer.Param4Ret;

	DbgDumpMsg(1, (PBYTE)Buffer, Length);
	Encrypt28((PUSHORT)Buffer, Length);
	if (ReadBytes) *ReadBytes = Length;
	return bRet;
}
