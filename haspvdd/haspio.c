/*  HASPVDD Driver 
 *
 *  (c) leecher@dose.0wnz.at 2022
 *
 *  This module implements the basic HASP IOCTL operations and conversion of
 *  the input buffer into the internal format, as well as encryption and 
 *  decryption of the data packets. 
 *  It gets uesd by usermode VDD (for buffer conversion) as well as Kernel mode 
 *  driver (for emulation of the classic ReadFile interface).
 *
 */
#if defined(_WINDOWS) || defined(_CONSOLE)
/* Usermode */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "log.h"
#undef RtlMoveMemory
EXTERN_C void _declspec(dllimport) WINAPI RtlMoveMemory(PVOID, const VOID*, SIZE_T);
#undef RtlZeroMemory
EXTERN_C void _declspec(dllimport) WINAPI RtlZeroMemory(PVOID, SIZE_T);
#else
/* Kernel mode */
#include <ntddk.h>
#include <windef.h>
#define IsBadReadPtr(p, len) (!MmIsAddressValid(p) || !MmIsAddressValid(p+len))
#define GetProcessHeap() (DWORD)-1
#define HeapAlloc(h,f,len) ExAllocatePool(PagedPool, len)
#define HeapFree(h,f,p) ExFreePool(p)
#define DbgDumpMsg(...)
#endif

#include "hasp.h"
#include "haspintl.h"
#include "haspcrypt.h"
#include "hardlockdrv.h"

/* Converts a decrypted 40 (0x28) byte DOS HASP buffer to the internal format 
 * used by the downlevel HARDLOCK.SYS driver 
 *
 * Parameters:
 *  Destination - Target structure where the conversion result gets put
 *  Src         - Already decrypted source DOS buffer 
 *  Length      - Size of the DOSBuffer part (40 - 4 = 36 bytes)
 *
 * Returns:
 *  1  - Error, i.e. invalid pointer to buffer
 *  0  - Success
 */
int Convert28ToHL(HaspWinBufferStruc* Destination, HaspBufferInStruc* Src, USHORT Length)
{
	if (!Src)
		return 1;

	RtlMoveMemory(&Destination->DOSBuffer, &Src->DOSBuffer, Length);

	Destination->Code1 = 1;
	Destination->Code2 = 1;
	Destination->Code3 = 0x6873686C;   // 'hshl'
	Destination->PacketLength = sizeof(HaspWinBufferStruc);
	Destination->InBufferLength = sizeof(HaspBufferInStruc);
	Destination->DOSBuffer.Service2 = Destination->DOSBuffer.Service;
	Destination->BufferOffset = Destination->DOSBuffer.AX;
	Destination->BufferSegment = Destination->DOSBuffer.ES;

	switch (Destination->DOSBuffer.Service)
	{
	case MEMOHASP_READBLOCK:
	case MEMOHASP_WRITEBLOCK:
	case LOCALHASP_ENCODEDATA:
	case LOCALHASP_DECODEDATA:
	case TIMEHASP_WRITEBLOCK:
	case TIMEHASP_READBLOCK:
		Destination->BlockLength = Destination->DOSBuffer.SI;
		break;
	}
	return 0;
}

/* Converts an unencrypted internal HASPLOCK.SYS buffer back to the DOS buffer
 * and encrypts the DOS buffer so that it can be fed back to the DOS 
 * application. Source buffer gets modified!

 *
 * Parameters:
 *  Src         - Reply buffer of HASPLOCK.SYS filled with return values
 *  Destination - Target DOS buffer to put result in.
 *  Length      - Size of the Destination Buffer (40 = 0x28 bytes)
 *
 * Returns:
 *  0 (so that caller can use it as FALSE return value)
 */
int Convert28FromHL(HaspWinBufferStruc* Src, HaspBufferInStruc* Destination, USHORT Length)
{
	Src->DOSBuffer.Param1 = Src->DOSBuffer.Param1Ret;
	Src->DOSBuffer.Param2 = Src->DOSBuffer.Param2Ret;
	Src->DOSBuffer.Param3 = Src->DOSBuffer.Param3Ret;
	Src->DOSBuffer.Param4 = Src->DOSBuffer.Param4Ret;
	Destination->DOSBuffer = Src->DOSBuffer;

	DbgDumpMsg(1, (PBYTE)Destination, Length);
	Encrypt28((PUSHORT)Destination, Length);

	return 0;
}

/* Dispatches an incoming, decrypted DOS 40 (0x28) byte buffer to the 
 * downlevel HARDLOCK.SYS driver
 *
 * Parameters:
 *  Buffer      - [IN ] The already decrypted DOS Buffer to process
 *  Length      - [IN ] Size of Buffer (40 = 0x28 bytes)
 *  ReadBytes   - [OUT] Number of bytes returned by the downlevel driver
 *
 * Returns:
 *  TRUE  - Operaion succeeded
 *  FALSE - Operation failed, buffer may still be filled with an error
 *          code in Param3 and can be passed on to the DOS application 
 */
BOOL HaspIOCtl(HaspBufferInStruc* Buffer, USHORT Length, PDWORD ReadBytes)
{
	BOOL ret = FALSE;
	HaspWinBufferStruc IOBuffer;
	DWORD MemBufLength;

	RtlZeroMemory(&IOBuffer, sizeof(IOBuffer));
	Convert28ToHL(&IOBuffer, Buffer, Length);
	switch (IOBuffer.DOSBuffer.Service2)
	{
	case MEMOHASP_READBLOCK:
	case MEMOHASP_WRITEBLOCK:
		IOBuffer.DOSBuffer.Param3Ret = (DWORD)HASPERR_MH_INVALID_ADDRESS;
		MemBufLength = IOBuffer.BlockLength * sizeof(USHORT);
		break;
	case LOCALHASP_ENCODEDATA:
	case LOCALHASP_DECODEDATA:
		IOBuffer.DOSBuffer.Param3Ret = (DWORD)HASPERR_INVALID_PARAMETER;
		MemBufLength = IOBuffer.BlockLength;
		break;
	case TIMEHASP_WRITEBLOCK:
	case TIMEHASP_READBLOCK:
		IOBuffer.DOSBuffer.Param3Ret = (DWORD)HASPERR_TH_INVALID_ADDRESS;
		MemBufLength = IOBuffer.BlockLength;
		break;
	default:
		MemBufLength = 0;
		break;
	}

	if (MemBufLength <= 512)
	{
		HaspWinBufferStruc* lpIOBuffer;

		if (MemBufLength > 0)
		{
			if (IsBadReadPtr((PBYTE)IOBuffer.BufferOffset, MemBufLength))
			{
				IOBuffer.DOSBuffer.Param3Ret = (DWORD)HASPERR_INVALID_POINTER;
				return Convert28FromHL(&IOBuffer, Buffer, Length);
			}
			lpIOBuffer = HeapAlloc(GetProcessHeap(), 0, sizeof(IOBuffer) + MemBufLength);
			if (!lpIOBuffer)
			{
				IOBuffer.DOSBuffer.Param3Ret = (DWORD)HASPERR_CANT_ALLOC_DOSMEM;
				return Convert28FromHL(&IOBuffer, Buffer, Length);
			}
			*lpIOBuffer = IOBuffer;
			switch (IOBuffer.DOSBuffer.Service2)
			{
			case MEMOHASP_WRITEBLOCK:
			case TIMEHASP_WRITEBLOCK:
			case LOCALHASP_ENCODEDATA:
			case LOCALHASP_DECODEDATA:
				RtlMoveMemory((PBYTE)lpIOBuffer + sizeof(IOBuffer), (PBYTE)IOBuffer.BufferOffset, MemBufLength);
				DbgDumpMsg(2, (PBYTE)lpIOBuffer + sizeof(IOBuffer), MemBufLength);
				break;
			}
		}
		else lpIOBuffer = &IOBuffer;

		//DbgDumpMsg(0, (PBYTE)lpIOBuffer, sizeof(IOBuffer) + MemBufLength);
		ret = CallHardlock(lpIOBuffer, sizeof(IOBuffer) + MemBufLength, ReadBytes);
		//DbgDumpMsg(1, (PBYTE)lpIOBuffer, sizeof(IOBuffer) + MemBufLength);

		if (MemBufLength > 0)
		{
			IOBuffer = *lpIOBuffer;
			if (IOBuffer.DOSBuffer.Param3Ret == HASPERR_SUCCESS)
			{
				switch (IOBuffer.DOSBuffer.Service2)
				{
				case MEMOHASP_WRITEBLOCK:
				case TIMEHASP_WRITEBLOCK:
				case LOCALHASP_ENCODEDATA:
				case LOCALHASP_DECODEDATA:
					RtlMoveMemory((PBYTE)IOBuffer.BufferOffset, lpIOBuffer + sizeof(IOBuffer), MemBufLength);
					DbgDumpMsg(2, (PBYTE)lpIOBuffer + sizeof(IOBuffer), MemBufLength);
					break;
				}
			}
			HeapFree(GetProcessHeap(), 0, lpIOBuffer);
		}
	}
	Convert28FromHL(&IOBuffer, Buffer, Length);
	return ret;
}
