/*  HASPVDD Driver 
 *
 *  (c) leecher@dose.0wnz.at 2022
 *
 *  This module implements communication with the next downlevel driver
 *
 */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winioctl.h>
#include "hasp.h"
#include "haspintl.h"
#include "hardlock.h"
#include "hardlockdrv.h"
#include "log.h"

/* This was just a test case to call HARDLOCK.SYS directly from usermode.
 * Unfortunately, this didn't work, as HASP_IOCTL_DOS_DISPATCH is only exposed
 * as IRP_MJ_INTERNAL_DEVICE_CONTROL, thus we cannot reach it from usermode.
 * This would has saved us from writing a kernel mode driver, d'oh! :(
 */
// #define HLDEV_DIRECT

#ifdef HLDEV_DIRECT
#define HARDLOCK_DEVNAME	"\\\\.\\GLOBALROOT\\Device\\FNT0"
#else
#define HARDLOCK_DEVNAME	"\\\\.\\Hasp"
#endif

static DWORD HaspVersionInstalled = 0;
static HANDLE HardlockDeviceHandle = INVALID_HANDLE_VALUE;

/* This initializes the downlevel driver. It would just be useful if we
 * would be able to interface with HARDLOCK.SYS directly.
 * Returns the currently installed version of the HARDLOCK.SYS driver
 * as caller requires to have a minimum version number for support.
 *
 * Parameters:
 *  pVersionInstalled  - [OUT] Version number of the installed HARDLOCK.SYS
 *
 * Returns:
 *  TRUE   - Success 
 *  FALSE  - Failed
 */
BOOL InitializeHardlock(PDWORD pVersionInstalled)
{
#ifdef HLDEV_DIRECT
	DWORD dwRead;

	return DeviceIoControl(HardlockDeviceHandle,
		HASP_IOCTL_DOS_INITIALIZE,
		pVersionInstalled, sizeof(DWORD),
		pVersionInstalled, sizeof(DWORD),
		&dwRead, NULL);
#else
	* pVersionInstalled = 339;
	return TRUE;
#endif
}

/* Checks, if the installed HARDLOCK.SYS driver is capable of handling
 * our calls
 *
 * Returns:
 *  TRUE   - Driver is capable of handling our calls
 *  FALSE  - Driver is too old
 */
BOOL CheckForHardlock(void)
{
	if (HaspVersionInstalled >= 235 || InitializeHardlock(&HaspVersionInstalled))
		return TRUE;
	return FALSE;
}

/* Opens a device handle to the underlying driver so that we can communicate
 * with it.
 * Does report error back to used, if global error settings allow it.
 *
 * Returns:
 *  TRUE   - Device handle opened, communication can start
 *  FALSE  - Opening device handle failed
 */
BOOL OpenHardlock(void)
{
	if (HardlockDeviceHandle == INVALID_HANDLE_VALUE)
	{
		HardlockDeviceHandle = CreateFile(HARDLOCK_DEVNAME,
			GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (HardlockDeviceHandle == INVALID_HANDLE_VALUE)
		{
			LogLastError("Cannot open HASP driver: ");
			return FALSE;
		}
	}
	return TRUE;
}

/* Closes the previously opened device handle to the underlying driver */
void CloseHardlock(void)
{
	if (HardlockDeviceHandle != INVALID_HANDLE_VALUE)
	{
		if (CloseHandle(HardlockDeviceHandle))
			HardlockDeviceHandle = INVALID_HANDLE_VALUE;
	}
}

/* Calls the underlying driver by passing Buffer to it for processing
 * Does report error back to used, if global error settings allow it.
 * Initializes underlying driver, if not initialized yet.
 * Uses the IOCTL interface, not the classic ReadFile interface, therefore
 * only works with our own driver.
 *
 * Parameters:
 *  Buffer    - [IN ] Buffer to process, gets read and filled by underlying 
 *                    driver 
 *  Length    - [IN ] Length of Buffer 
 *  ReadBytes - [OUT] Number of bytes read from underlying driver 
 * Returns:
 *  TRUE      - Operation successful
 *  FALSE     - Failed 
 */
BOOL CallHardlock(HaspWinBufferStruc* Buffer, ULONG Length, PDWORD ReadBytes)
{
	if (!OpenHardlock())
	{
		Buffer->DOSBuffer.Param3Ret = HASPERR_CANT_OPEN_HDD;
		return FALSE;
	}

	/* If necessary, read HASPNT service setting from registry
	 * This opens HKEY_LOCAL_MACHINE\System\CurrentControlSet\Services\HaspNt
	 * key and reads its settings to configure HARDLOCK.SYS operation
	 */
	if (CheckForHardlock())
	{
		if (HaspVersionInstalled < 235)
		{
			char szMsg[256];

			wsprintf(szMsg, "Invalid HASP version on initialization: %d",
				HaspVersionInstalled);
			LogError(szMsg);
			Buffer->DOSBuffer.Param3Ret = HASPERR_CANT_OPEN_HDD;
			return FALSE;
		}
		if (!DeviceIoControl(HardlockDeviceHandle,
			HASP_IOCTL_DOS_DISPATCH,
			Buffer, Length,
			Buffer, Length,
			ReadBytes, NULL))
		{
			LogLastError("HASP Request failed: ");
			Buffer->DOSBuffer.Param3Ret = HASPERR_CANT_OPEN_HDD;
			return FALSE;
		}

		return TRUE;
	}
	return FALSE;
}

/* Calls the underlying driver by passing Buffer to it for processing
 * Does report error back to used, if global error settings allow it.
 * Initializes underlying driver, if not initialized yet.
 * Uses the classic ReadFile interface, 
 *
 * Parameters:
 *  Buffer    - [IN ] Buffer to process, gets read and filled by underlying 
 *                    driver 
 *  Length    - [IN ] Length of Buffer 
 *  ReadBytes - [OUT] Number of bytes read from underlying driver 
 * Returns:
 *  TRUE      - Operation successful
 *  FALSE     - Failed 
 */
BOOL CallLegacyHardlock(HaspBufferInStruc* Buffer, ULONG Length, PDWORD ReadBytes)
{
	BOOL Success;

	if (!OpenHardlock()) return FALSE;
	if (!(Success = ReadFile(HardlockDeviceHandle, Buffer, Length, ReadBytes, NULL)))
		LogLastError("HASP Request failed: ");
	CloseHardlock();
	return Success;
}
