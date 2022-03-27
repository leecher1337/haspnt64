#pragma once
void Decrypt28(PUSHORT Buffer, int Length);
void Encrypt28(PUSHORT Buffer, int Length);
BOOL HaspIOCtl(HaspBufferInStruc* Buffer, USHORT Length, PDWORD ReadBytes);
