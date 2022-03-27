#pragma once
BOOL CheckForHardlock(void);
BOOL InitializeHardlock(PDWORD pVersionInstalled);
void CloseHardlock(void);
int CallHardlock(HaspWinBufferStruc* Buffer, ULONG Length, PDWORD ReadBytes);
BOOL CallLegacyHardlock(HaspBufferInStruc* Buffer, ULONG Length, PDWORD ReadBytes);


