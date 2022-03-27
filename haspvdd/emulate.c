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

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "hasp.h"
#include "haspintl.h"
#include "haspio.h"
#include "log.h"

HaspDOSBufferStruc EmulatorCodeTemplate;

/* Initializes the emulator by loading its configuration
 *
 * Parameters:
 *  hKey - Registry key of the node containing the configuration settings
 *
 * Returns:
 *  TRUE  - Initialization succeeded
 *  FALSE - Failure to read configuration 
 */
BOOL EmulatorInit(HKEY hKey)
{
	DWORD cbData = 4 * sizeof(DWORD);

	return RegQueryValueEx(hKey, "EmulateParams", NULL, NULL, 
		(LPBYTE)&EmulatorCodeTemplate.Param1Ret, &cbData) == ERROR_SUCCESS &&
		cbData == 4 * sizeof(DWORD);
}

/* Emulates a call to the HASP dongle, dispatches incoming calls.
 * Expects a decrypted buffer in HaspBufferInStruc
 * Encrypts the buffer after filling it so that it can be passed on
 * to the next driver.
 *
 * Parameters:
 *  Buffer    - [IN]  Decrypoted Buffer from caller to check
 *  Length    - [IN]  Length of Buffer 
 *  ReadBytes - [OUT] Numbre of bytes read 
 *
 * Returns:
 *  TRUE      - Incoming packet has been processed successfully
 *  FALSE     - Packet could not be processed, onknown Service code
 *              Even on FALSE, return Buffer is being Re-encrypted
 */
BOOL EmulateCalls(HaspBufferInStruc* Buffer, USHORT Length, PDWORD ReadBytes)
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
