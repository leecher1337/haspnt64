/*  HASPNT 64 bit driver
 *
 *  (c) leecher@dose.0wnz.at 2022
 *
 * Main module that dispatches all calls and brings them to the appropriate 
 * routine
 *
 */
#include <ntddk.h>
#include <windef.h>
#include "hardlock.h"
#include "haspintl.h"
#include "haspio.h"
#include "haspcrypt.h"
#include "hardlockdrv.h"

//----------------------------------------------------------------------
//                         GLOBALS
//----------------------------------------------------------------------

PDEVICE_OBJECT deviceObject;
PCHAR	HaspBuffer;
ULONG	HaspBufferLength;

#define STR_DEVNAME	(L"\\Device\\Hasp")
#define STR_DOSDEVNAME	(L"\\DosDevices\\hasp")

NTSTATUS HASP_EMULATOR(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp);
NTSTATUS IRP_READ_AND_DIOC(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp);
NTSTATUS HASP_SWITCHER(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp);
VOID CancelFunction(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp);
NTSTATUS HASP_UNLOAD(IN PDRIVER_OBJECT DriverObject);

#if DBG
#define NTPrint DbgPrint
#else
#define NTPrint(...)
#endif

//----------------------------------------------------------------------
//
// DriverEntry
//
// Installable driver initialization. Here we just set ourselves up.
//
// This routine will handle the initialization of this driver. There is
// not a lot of code required other than to initialize the device object
// and extension, then return.
//
//----------------------------------------------------------------------
NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
    NTSTATUS                ntStatus;
    WCHAR                   deviceNameBuffer[] = STR_DEVNAME;
    UNICODE_STRING          deviceNameUnicodeString;
    WCHAR                   deviceLinkBuffer[] = STR_DOSDEVNAME;
    UNICODE_STRING          deviceLinkUnicodeString;

    UNREFERENCED_PARAMETER(RegistryPath);

    //
    //
    //
    // Setup the device name
    //
    RtlInitUnicodeString(&deviceNameUnicodeString, deviceNameBuffer);
    //
    // Create the device used for GUI communications
    //
    ntStatus = IoCreateDevice(DriverObject,
        0,
        &deviceNameUnicodeString,
        FILE_DEVICE_PARALLEL_PORT,
        0,
        FALSE,
        &deviceObject);
    
    if (!NT_SUCCESS(ntStatus))
    {
        NTPrint("IoCreateDevice failed: %X", ntStatus);
        return ntStatus;
    }

    DriverObject->MajorFunction[IRP_MJ_CREATE] = HASP_SWITCHER;
    DriverObject->MajorFunction[IRP_MJ_CLOSE] = HASP_SWITCHER;
    DriverObject->MajorFunction[IRP_MJ_READ] = HASP_SWITCHER;
    DriverObject->MajorFunction[IRP_MJ_WRITE] = HASP_SWITCHER;
    DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = HASP_SWITCHER;
    DriverObject->MajorFunction[IRP_MJ_INTERNAL_DEVICE_CONTROL] = HASP_SWITCHER;
    DriverObject->DriverStartIo = HASP_EMULATOR;
    DriverObject->DriverUnload = HASP_UNLOAD;
    DriverObject->Flags |= DO_BUFFERED_IO;

    //
    // If successful, make a symbolic link that allows for the device
    // object's access from Win32 programs
    //
    RtlInitUnicodeString(&deviceLinkUnicodeString, deviceLinkBuffer);
    ntStatus = IoCreateSymbolicLink(&deviceLinkUnicodeString, &deviceNameUnicodeString);


    //
    // If something went wrong, cleanup the device object and don't load
    //

    if (!NT_SUCCESS(ntStatus))
    {

        NTPrint("IoCreateSymbolicLink failed: %08X", ntStatus);
        NTPrint("Failed to create our device!");

        if (deviceObject)
        {
            IoDeleteDevice(DriverObject->DeviceObject);
        }
        return (STATUS_UNSUCCESSFUL);
    }

    return STATUS_SUCCESS;
}
//----------------------------------------------------------------------
//
// HASP_Unload
//
// Our job is done - time to leave.
//
//----------------------------------------------------------------------
NTSTATUS HASP_UNLOAD(IN PDRIVER_OBJECT DriverObject)
{
    WCHAR                  deviceLinkBuffer[] = STR_DOSDEVNAME;
    UNICODE_STRING         deviceLinkUnicodeString;

    //
    // Delete the symbolic link for our GUI device
    //
    RtlInitUnicodeString(&deviceLinkUnicodeString, deviceLinkBuffer);
    IoDeleteSymbolicLink(&deviceLinkUnicodeString);

    //
    // Delete the device object, making sure that the GUI device
    // object is always deleted.
    //
    IoDeleteDevice(DriverObject->DeviceObject);

    return(STATUS_SUCCESS);
}
//----------------------------------------------------------------------
//
//
// Routine Description:
//
//  This routine is used to cancel any request in the driver.
//
// Arguments:
//
//    DeviceObject - Pointer to the device object for this device
//
//    Irp - Pointer to the IRP to be canceled.
//
// Return Value:
//
//    None.
//
//----------------------------------------------------------------------

VOID CancelFunction(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{

    //
    // If this is the current request then just let normal
    // code detect that the current is cancelled.
    //
    // If this isn't the current request then remove the request
    // from the device queue and complete it.
    //

    if (Irp != DeviceObject->CurrentIrp) {

        KeRemoveEntryDeviceQueue(
            &DeviceObject->DeviceQueue,
            &Irp->Tail.Overlay.DeviceQueueEntry
        );

        Irp->IoStatus.Status = STATUS_CANCELLED;
        Irp->IoStatus.Information = 0;

        IoReleaseCancelSpinLock(Irp->CancelIrql);

        IoCompleteRequest(Irp, IO_NO_INCREMENT);

    }
    else {

        IoReleaseCancelSpinLock(Irp->CancelIrql);

    }

}

//----------------------------------------------------------------------
//
//
// Routine Description:
//
//  This routine dispatches the IOCTLs sent by the caller to the
//  appropriate functions, i.e. to the downlevel HARDLOCK.SYS driver
//
// Arguments:
//
//    DeviceObject - Pointer to the device object for this device
//
//    Irp - Pointer to the IRP.
//
// Return Value:
//
//    NTSTATUS Status code of the operation.
//
//----------------------------------------------------------------------

NTSTATUS HASP_SWITCHER(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
    NTSTATUS ntStatus = STATUS_SUCCESS;
    Irp->IoStatus.Information = 0;

    switch (Irp->Tail.Overlay.CurrentStackLocation->MajorFunction)
    {
    case IRP_MJ_CREATE:
        if (!CheckForHardlock())
        {
            NTPrint("Hardlock not found");
            ntStatus = STATUS_INVALID_PARAMETER;
        }
        break;
    case IRP_MJ_DEVICE_CONTROL:
        if (Irp->Tail.Overlay.CurrentStackLocation->Parameters.DeviceIoControl.IoControlCode == HASP_IOCTL_DOS_DISPATCH)
        {
            if (Irp->Tail.Overlay.CurrentStackLocation->Parameters.DeviceIoControl.InputBufferLength !=
                Irp->Tail.Overlay.CurrentStackLocation->Parameters.DeviceIoControl.OutputBufferLength ||
                Irp->Tail.Overlay.CurrentStackLocation->Parameters.DeviceIoControl.InputBufferLength <
                sizeof(HaspWinBufferStruc))
            {
                NTPrint("Invalid input parameters for HASP_IOCTL_DOS_DISPATCH: LenIn=%d, LenOut=%d, Expected=%d",
                    Irp->Tail.Overlay.CurrentStackLocation->Parameters.DeviceIoControl.InputBufferLength,
                    Irp->Tail.Overlay.CurrentStackLocation->Parameters.DeviceIoControl.OutputBufferLength,
                    sizeof(HaspWinBufferStruc)
                );

                ntStatus = STATUS_INVALID_PARAMETER;
                break;
            }
            CallHardlock((HaspWinBufferStruc*)Irp->AssociatedIrp.SystemBuffer,
                Irp->Tail.Overlay.CurrentStackLocation->Parameters.DeviceIoControl.InputBufferLength,
                NULL);
            Irp->IoStatus.Information = Irp->Tail.Overlay.CurrentStackLocation->Parameters.DeviceIoControl.OutputBufferLength;
            break;
        }
    case IRP_MJ_READ:
        return IRP_READ_AND_DIOC(DeviceObject, Irp);
    }

    Irp->IoStatus.Status = ntStatus;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    return(ntStatus);
}

//----------------------------------------------------------------------
//
//
// Routine Description:
//
//  This routine processes the IRP_MJ_READ call (classic interface) and 
//  dispatches the READ operation.
//
// Arguments:
//
//    DeviceObject - Pointer to the device object for this device
//
//    Irp - Pointer to the IRP.
//
// Return Value:
//
//    NTSTATUS Status code of the operation.
//
//----------------------------------------------------------------------

NTSTATUS IRP_READ_AND_DIOC(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
    HaspBuffer = Irp->UserBuffer;
    if (Irp->Tail.Overlay.CurrentStackLocation->Parameters.Read.Length == 0x28)
    {
        HaspBufferLength = Irp->Tail.Overlay.CurrentStackLocation->Parameters.Read.Length;
        IoStartPacket(DeviceObject, Irp, NULL, CancelFunction);
    }
    else
    {
        NTPrint("Invalid length: %d", Irp->Tail.Overlay.CurrentStackLocation->Parameters.Read.Length);
        HaspBuffer[4] = (UCHAR)0xD8;
        IoCompleteRequest(Irp, IO_NO_INCREMENT);
    }

    return(STATUS_SUCCESS);
}

//----------------------------------------------------------------------
//
//
// Routine Description:
//
//  This routine startes reading on a IRP_MJ_READ call (classic interface)
//  and dispatches the READ operation, if a complete packet has been 
//  received
//
// Arguments:
//
//    DeviceObject - Pointer to the device object for this device
//
//    Irp - Pointer to the IRP.
//
// Return Value:
//
//    NTSTATUS Status code of the operation.
//
//----------------------------------------------------------------------

NTSTATUS HASP_EMULATOR(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
    NTPrint("Entering Emu, Buffer length is %2.2lx bytes", HaspBufferLength);
    if (HaspBufferLength == sizeof(HaspDOSBufferStruc))
    {
        Decrypt28((PUSHORT)HaspBuffer, (USHORT)HaspBufferLength);
        HaspIOCtl((HaspBufferInStruc*)HaspBuffer, (USHORT)HaspBufferLength, NULL);
    }
    IoStartNextPacket(DeviceObject, TRUE);
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    return(STATUS_SUCCESS);
}


