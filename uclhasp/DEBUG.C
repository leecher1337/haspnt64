//----------------------------------------------------------------------
//
// support address meteo@null.net
//
//----------------------------------------------------------------------
//
// Debug messages to Debugger
//
//----------------------------------------------------------------------
#include "hasp.h"
#include <stdio.h>
#include <stdarg.h>
#include <Shlwapi.h>
//----------------------------------------------------------------------
extern HaspBufferInStruc *HaspBuffer;

char szHaspInReg[] ="->EAX=%8.8lx BH=%2.2lx Pwd=%4.4lx:%4.4lx EDI=%8.8lx ESI=%8.8lx";
char szHaspOutReg[]="<-EAX=%4.4lx EBX=%4.4lx ECX=%4.4lx EDX=%4.4lx";

VOID NTPrint (char * szFormat, ...)
{
	char buf[256];
	va_list va;

	va_start( va, szFormat );
    wvnsprintfA( buf, sizeof(buf), szFormat, va );
	va_end( va );
	OutputDebugStringA (buf);
}

VOID __stdcall DumpInRegs (VOID)
{

	__asm{
                push	eax
                xor	eax, eax

                push    [ebp]HaspBufferInStruc.__ESI
		push	[ebp]HaspBufferInStruc.__EDI
                
                mov	ax, word ptr ([ebp]HaspBufferInStruc.__EDX)
		push	eax

		mov	ax, word ptr ([ebp]HaspBufferInStruc.__ECX)
                push	eax

                movzx	eax, byte ptr [ebp]HaspBufferInStruc.__BH
                push	eax
                push    [ebp]HaspBufferInStruc.__EAX	; Trace out HASP call params
                push	offset szHaspInReg
                call	NTPrint
                add	esp, 7*4

                pop	eax
             }

}

VOID __stdcall DumpOutRegs (VOID)
{
	__asm{
                and	eax, 0ffffh
                and	ebx, 0ffffh
                and	ecx, 0ffffh
                and	edx, 0ffffh
        	push	edx
                push	ecx
                push	ebx
                push	eax
                push	offset szHaspOutReg
                call	NTPrint
                add	esp, 5*4	; stack size
             }
}
