#pragma once
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "haspintl.h"
enum {
	MODE_IOCTLIF = 0,		// IOCTL interface to interface with our HASPNT64 driver (default)
	MODE_LEGACYIF = 1,		// Legacy ReadFile() based interface used by the original driver
	MODE_EMULATE = 2,		// Just "Emulate" call LOCALHASP_HASPCODE with fixed reply
	MODE_EMULATEDLL = 3		// Use loaded emulation DLL
} EmulatorMode;

typedef BOOL(__stdcall* fnHaspIOCtl)(HaspBufferInStruc* Buffer, ULONG Length, PDWORD ReadBytes);
