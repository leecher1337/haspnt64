/*  HASPNT 64 bit driver
 *
 *  (c) leecher@dose.0wnz.at 2022
 *
 * Module to pass on incoming IOCTLs to the downlevel HARDLOCK.SYS driver
 * via IRP_MJ_INTERNAL_DEVICE_CONTROL
 * This exposes the same interface as hardlockdrv.c in the VDD
 *
 */
#include <ntddk.h>
#include <windef.h>
#include "hardlock.h"
#include "haspintl.h"
#include "haspapi.h"

#define STR_HARDLOCKDEV L"\\Device\\FNT0"

static DWORD HaspVersionInstalled = 0;

/* This initializes the downlevel driver. 
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
    NTSTATUS Status;
    IRP* pIRP;
    KEVENT Event;
    UNICODE_STRING HardlockDevName;
    IO_STATUS_BLOCK IoStatusBlock;
    PDEVICE_OBJECT DeviceObject;
    PFILE_OBJECT FileObject;

    KeInitializeEvent(&Event, NotificationEvent, 0);
    RtlInitUnicodeString(&HardlockDevName, STR_HARDLOCKDEV);
    Status = IoGetDeviceObjectPointer(&HardlockDevName, FILE_ALL_ACCESS, &FileObject, &DeviceObject);
    if (NT_SUCCESS(Status))
    {
        pIRP = IoBuildDeviceIoControlRequest(
            HASP_IOCTL_DOS_INITIALIZE,
            DeviceObject,
            pVersionInstalled,
            4u,
            pVersionInstalled,
            4u,
            FALSE,
            &Event,
            &IoStatusBlock);
        if (pIRP)
        {
            if (IofCallDriver(DeviceObject, pIRP) == STATUS_PENDING)
                KeWaitForSingleObject(&Event, Executive, 0, 0, 0);
            ObfDereferenceObject(FileObject);
            Status = IoStatusBlock.Status;
        }
        else
        {
            ObfDereferenceObject(FileObject);
            Status = STATUS_INVALID_PARAMETER;
        }
    }
    return NT_SUCCESS(Status);
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

/* Calls the underlying driver by passing Buffer to it for processing.
 * Initializes underlying driver, if not initialized yet.
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
    NTSTATUS Status;
    IRP* pIRP;
    KEVENT Event;
    UNICODE_STRING HardlockDevName;
    IO_STATUS_BLOCK IoStatusBlock;
    PDEVICE_OBJECT DeviceObject;
    PFILE_OBJECT FileObject;

    if (CheckForHardlock())
    {
        KeInitializeEvent(&Event, NotificationEvent, 0);
        RtlInitUnicodeString(&HardlockDevName, STR_HARDLOCKDEV);
        Status = IoGetDeviceObjectPointer(&HardlockDevName, FILE_ALL_ACCESS, &FileObject, &DeviceObject);
        if (NT_SUCCESS(Status))
        {
            pIRP = IoBuildDeviceIoControlRequest(
                HASP_IOCTL_DOS_DISPATCH,
                DeviceObject,
                Buffer,
                Length,
                Buffer,
                Length,
                TRUE,
                &Event,
                &IoStatusBlock);
            if (pIRP)
            {
                if (IofCallDriver(DeviceObject, pIRP) == STATUS_PENDING)
                    KeWaitForSingleObject(&Event, Executive, 0, 0, 0);
                ObfDereferenceObject(FileObject);
                Status = IoStatusBlock.Status;
            }
            else
            {
                ObfDereferenceObject(FileObject);
                Status = STATUS_INVALID_PARAMETER;
            }
            if (NT_SUCCESS(Status))
            {
                if (ReadBytes) *ReadBytes = (DWORD)IoStatusBlock.Information;
                return TRUE;
            }
        }
    }
    Buffer->DOSBuffer.Param3Ret = (DWORD)HASPERR_CANT_OPEN_HDD;
    return FALSE;
}

