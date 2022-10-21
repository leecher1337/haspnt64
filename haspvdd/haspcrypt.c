#if defined(_WINDOWS) || defined(_CONSOLE)
/* Usermode */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
/* Kernel mode */
#include <ntddk.h>
#include <windef.h>
#endif

#include "haspintl.h"

/* Decrypts the DOS 40 (0x28) byte input buffer in-place
 *
 * Parameters:
 *  Buffer    - The buffer to decrypt
 *  Length    - Length of the buffer, currently ignored, as alwawys 40 bytes
 */
void Decrypt28(PUSHORT Buffer, int Length)
{
	USHORT register v2, v4, v6;
	int i, j;

	UNREFERENCED_PARAMETER(Length);
	for (i = 0, v2 = 17; i < sizeof(HaspBufferInStruc) / sizeof(USHORT); i++)
	{
		for (j = 0, v6 = 0, v4 = Buffer[i]; j < 16; j++)
		{
			v6 |= (((BYTE)v4 ^ (BYTE)v2) & 1) << j;
			v2 = (v2 >> 1) ^ ('IQ' * (v4 & 1));
			v4 >>= 1;
		}
		Buffer[i] = v6;
	}
}

/* Encrypts the DOS 40 (0x28) byte input buffer in-place
 *
 * Parameters:
 *  Buffer    - The buffer to encrypt
 *  Length    - Length of the buffer, currently ignored, as alwawys 40 bytes
 */
void Encrypt28(PUSHORT Buffer, int Length)
{
	USHORT register v2, v4, v6;
	int i, j;

	UNREFERENCED_PARAMETER(Length);
	for (i = 0, v2 = 17; i < sizeof(HaspBufferInStruc) / sizeof(USHORT); i++)
	{
		for (j = 0, v6 = 0, v4 = Buffer[i]; j < 16; j++)
		{
			v6 |= (((BYTE)v4 ^ (BYTE)v2) & 1) << j;
			v2 = (v2 >> 1) ^ ('IQ' * (((BYTE)v4 ^ (BYTE)v2) & 1));
			v4 >>= 1;
		}
		Buffer[i] = v6;
	}
}
