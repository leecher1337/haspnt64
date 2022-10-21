#pragma once
BOOL EmulatorInit(HKEY hKey, DWORD* pInterfaceMode, fnHaspIOCtl* pfnHaspIOCtl);
BOOL EmulateCalls(HaspBufferInStruc* Buffer, USHORT Length, PDWORD ReadBytes);

