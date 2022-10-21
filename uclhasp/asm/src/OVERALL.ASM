;
; support address meteo@null.net
;
;
; Miscellanious routines
;
.386p
;
include w32.inc
include	haspdef.inc
include haspfunc.inc
include misc.inc
;
;
_TEXT		SEGMENT PUBLIC 'CODE' USE32
		ASSUME  CS:_TEXT, DS:_TEXT
;
		GLOBAL	_heapSupported	: DWORD
		GLOBAL	_t_Supported	: DWORD
		GLOBAL	Prog_type	: DWORD
		GLOBAL	dwAddValue	: DWORD ; used for protection
		GLOBAL _GetOffsetToMem@0: PROC
;
;ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß
; Determines which key are used
;
BeginProc	_IsKeySupported@0
		cld				; which HASP we'll f..ck
                mov	esi, [_heapSupported]
		movzx	ebx, word ptr [ebp.__ECX]	; 100% bug support
IFNDEF REGISTERED
                add	ebx, [dwAddValue]		; Protection
ENDIF
		mov     ecx, dword ptr [_t_Supported]	; Total #of progs
		or	ecx, ecx			; Protection
		jz	@t_s0
	@loop:
		lodsd				; ds:esi -> ax
		cmp	eax, ebx
		je	@fnx
		add	esi, 4*3		; next value
		loop	@loop

	@fnx:
        	dec	ecx
		sub	ecx, dword ptr [_t_Supported]
		neg	ecx

		cmp	ecx, dword ptr [_t_Supported]
		ja	@read			; branch if (cx<[t_supported])

		clc
	@@done:
		mov	dword ptr Prog_type, ecx
	ret
;------------------------------------------------------------------------
	@t_s0:
        @read:
		stc
		mov	ecx, -1
		jmp	short @@done
EndProc _IsKeySupported@0
;
;
;ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß
; Input:	eax=#of program
; Return :      ebx=offset to memory
;
BeginProc _GetOffsetToMem@0
BeginProc GetOffsetToMem
		cld
		push	esi
                push	eax
                push	ecx

                dec	eax
		mov	ecx, eax	; Loop counter
                xor	ebx, ebx	; Initial offset
                mov	esi, [_heapSupported]
		or	ecx, ecx	; If #of prog=0 (e.g start of PwdTable)
                jz	@zero

	@xloop:
		add	esi, 3*4
                lodsd			; Get Type
		call	GetMemSize
		add	eax, 4		; Add SN value
                add	ebx, eax
        	loop	@xloop
	@zero:
        	pop	ecx
        	pop	eax
                pop	esi
		ret
EndProc GetOffsetToMem
EndProc _GetOffsetToMem@0
;
;ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß
; Input: EAX=#of program
; OutPut: EAX=Dongle type (usually used by GetHASPstatus)
;
BeginProc GetDongleType
		cld
		push	esi
                push	ecx

                dec	eax
		xchg	ecx, eax	; Loop counter
                mov	esi, _heapSupported
		lea	esi, [esi.Cell._Type]
		lodsd			; Get 1st dongle
		or	ecx, ecx	; If #of prog=0 (e.g start of PwdTable)
                jz	@zeroed

	@sloop:
		add	esi, 3*4
                lodsd			; Get Type
        	loop	@sloop		; 1 - MemoHASP (112)
	@zeroed:                        ; 4 - MemoHASP4 (496)
        	pop	ecx             ; 0 - other keys
                pop	esi
		ret
EndProc GetDongleType
;
;ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß
; Retrieve HASP memory size by value of GetHASPtype
;
; Input: AX: HASP type (value by GetHASPType)
; Out:   AX: Size of memory
;
BeginProc	GetMemSize		;
		clc         		; case ax:
		dec	eax
                jz	@112
                dec	eax
                jz	@unk
                dec	eax
		jz	@unk
                dec	eax
                jz	@496
      	@unk:
        	stc
                ret
	@496:
        	mov	eax, 496	;
                ret
	@112:
        	mov	eax, 112
                ret
EndProc	GetMemSize
;
; Converts AX to Ascii value which placed at buffer ES:EDI
;
BeginProc Cvt2Hex
		push	cx
		mov	cx,0404h	;For AF clear can use   "mov  CX,0504"
			     		;Clear AF		"dec  CH"
	$$c:    rol	ax, cl		;Doesnt change AF
		push	ax
        	mov	ah, 30h
        	and	al, 0fh		;For 0Fxh case, else to AH add 1
        	aaa
		db	0d5h, 11h
		stosb
        	pop	ax
		dec	ch		;clear AF
		jnz	$$c
		pop	cx
	ret
EndProc Cvt2Hex
;
_TEXT ENDS
END
