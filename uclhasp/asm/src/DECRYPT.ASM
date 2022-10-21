;
; support address meteo@null.net
;

; 06.08.1999 20:39
;
; Decrypt/Call_HASP routines
;
.386p
.MODEL FLAT, StdCall
;
include w32.inc
include haspfunc.inc
;
_TEXT		SEGMENT PUBLIC 'CODE' USE32
		ASSUME  CS:_TEXT,DS:_DATA
;
		EXTRN	Call_HASP	:PROC
;
;ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß
; This function proceed with Buffer.CryptType:=0x48
;
; Input: ebp - inbuffer
;
BeginProc _Buffer0x48@0
BeginProc Buffer0x48
		pushad
		call	EncryptROR		; Decrypt param

		call	Call_HASP

		mov     [ebp.o_EAX],eax		; Return values
		mov     [ebp.o_EBX],ebx
		mov     [ebp.o_ECX],ecx
		mov     [ebp.o_EDX],edx

		call	EncryptROR		; Encrypt return param
                popad
	ret
EndProc Buffer0x48
EndProc _Buffer0x48@0
;ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß
;
; This function proceeds with Buffer.CryptType:=0x28
;
; At old HASP drv (Old_API_Support). Was written at 1.04.006
; Input: ebp - inbuffer
;
BeginProc _Buffer0x28@0
BeginProc Buffer0x28
		pushad
		mov	edi, ebp
		call	Decrypt28
		sub	ebp,12h
; Convert params
;
		shr	dword ptr [ebp.__ECX], 10h	; password
		shr	dword ptr [ebp.__EDX], 10h

		shr	dword ptr [ebp.__ESI], 10h	; Cvt ESI
		shr	dword ptr [ebp.__EDI], 10h	; Cvt EDI

		shr	dword ptr [ebp.__EAX], 10h	; Cvt EAX

	@block:
;+3.00.506
;		mov	eax, [edi.i_MapESAX]		; ES:EAX
;
		mov	eax, [ebp+36h]
;-3.00.506
		mov	[ebp+40h], eax

		push	edi
		call	Call_HASP
		pop	edi
		mov     [edi.v_EAX], eax	; Return values
		mov     [edi.v_EBX], ebx	; Return values
		mov     [edi.v_ECX], ecx	; Return values
		mov     [edi.v_EDX], edx	; Return values
		call	Encrypt28
		popad
	ret
EndProc Buffer0x28
EndProc _Buffer0x28@0
;ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß
;
; This function proceeds with Buffer.CryptType:=0x54
;
; Input: ebp - inbuffer
;
BeginProc _Buffer0x54@0
BeginProc Buffer0x54
		pushad

		add	ebp, 4
		mov	edi, ebp
		mov	esi, offset32 _m_Buf	; Tmp Buf
		call	Decrypt54
		mov	ebp, esi		; Decrypted tmp buffer
		sub	ebp, 12h
		push	esi
		push	edi
                           				; ** Exchange params **
		shr	dword ptr [ebp.__ECX], 10h	; password
		shr	dword ptr [ebp.__EDX], 10h

		shr	dword ptr [ebp.__ESI], 10h	; Cvt ESI (shift right)
		shr	dword ptr [ebp.__EDI], 10h	; Cvt EDI
		shr	dword ptr [ebp.__EAX], 10h	; Cvt EAX
		mov	eax, dword ptr [ebp+36h]
		mov	dword ptr [ebp+40h], eax
		call	Call_HASP
		pop	esi
		pop	edi
		mov     [edi.v_EAX], eax	; Return values
		mov     [edi.v_EBX], ebx	; Return values
		mov     [edi.v_ECX], ecx	; Return values
		mov     [edi.v_EDX], edx	; Return values

		call	Encrypt54

                popad
	ret
EndProc Buffer0x54
EndProc _Buffer0x54@0
;
;ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß
BeginProc _EncryptROR@0
BeginProc EncryptROR
		mov	ecx, 28h
	@xor_p:
		xor	byte ptr [ebp+ecx-1+14h], 0AAh	; decrypt params
		ror	byte ptr [ebp+ecx-1+14h], 4
		loop	@xor_p
		ret
EndProc EncryptROR
EndProc _EncryptROR@0
;ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß
; Encrypts out buffer
; Input: EDI - buffer for encrypt params
;
BeginProc _Encrypt28@0
BeginProc Encrypt28
LOCAL	Final_counter:word,counter:word,Cur_Word:word,tmp_decrypt:word,var_1:word,var_2:word,var_3:word

		pushad
		xor	esi, esi
		mov	word ptr counter, 0
		mov	word ptr Final_counter, 28h
        mov	word ptr tmp_decrypt, 11h
@encr_init:
		mov	si, counter
		mov	ax, [esi+edi]
		mov	Cur_Word, ax
		mov	var_1, 0
		mov	var_2, 0
@encr_loop:
		mov	bx, tmp_decrypt
		mov	cx, Cur_Word
		xor	bx, cx
		and	ebx, 1
		mov	var_3,	bx
		mov	esi, ebx
		mov	cx, word ptr var_1
		shl	si, cl
		mov	ax, var_2
		or	ax, si
		mov	var_2, ax
		mov	eax, 'IQ'		; so... cool! ;) IQ=0
		mul	var_3
		mov	cx, ax
		mov	ax, tmp_decrypt
		sar	ax, 1
		mov	ebx, eax
		xor	bx, cx
		mov	tmp_decrypt, bx
		mov	ax, Cur_Word
		sar	ax, 1
		mov	Cur_Word, ax
		inc	var_1
		mov	eax, 10h
		cmp	ax, word ptr var_1
		jnz	@encr_loop
		mov	ax, var_2
		mov	si, counter
		mov	[esi+edi], ax
		add	counter, 2
		mov	ax, Final_counter
		cmp	ax, counter
		jnz	@encr_init
		popad
		ret
EndProc Encrypt28
EndProc _Encrypt28@0
;ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß
; Decrypts in buffer
; Input: EDI - buffer for decrypt params
;
BeginProc _Decrypt28@0
BeginProc Decrypt28
LOCAL	Final_counter:word,counter:word,Cur_Word:word,tmp_decrypt:word,var_1:word,var_2:word,var_3:word
		pushad
		xor	esi, esi
		mov	counter, 0
		mov 	Final_counter, 28h
        mov	word ptr tmp_decrypt, 11h
@decr_init:
		mov	si, counter
		mov	ax, [esi+edi]
		mov	Cur_Word, ax
		mov	var_1,	0
		mov	var_2,	0

@decr_loop:
		mov	bx, tmp_decrypt
		mov	cx, Cur_Word
		xor	bx, cx
		and	ebx, 1
		mov	si, bx
		mov	cx, var_1
		shl	si, cl
		mov	ax, var_2
		or	ax, si
		mov	var_2,	ax
		mov	ax, Cur_Word
		and	eax, 1
		mov	ebx, 'IQ'
		mul	bx
		mov	cx, ax
		mov	ax, tmp_decrypt
		sar	ax, 1
		mov	bx, ax
		xor	bx, cx
		mov	tmp_decrypt, bx
		mov	ax, Cur_Word
		sar	ax, 1
		mov	Cur_Word, ax
		inc	var_1
		mov	ax, 10h
		cmp	ax, var_1
		jnz	@decr_loop
		mov	ax, var_2
		mov	si, counter
		mov	[esi+edi], ax
		add	counter, 2
		mov	ax, Final_counter
		cmp	ax, counter
		jnz	@decr_init

		popad
		ret

EndProc Decrypt28
EndProc _Decrypt28@0
;ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß
; Encrypts out buffer
; Input: EDI - buffer for encrypt params
; Out: ESI - store buffer
;
BeginProc _Encrypt54@0
BeginProc Encrypt54
		pushad
		push	edi
		xchg	esi, edi
		call	Encrypt54x
		pop	esi
		call	Encrypt54x
		popad
		ret
EndProc Encrypt54
EndProc _Encrypt54@0
;***
BeginProc Encrypt54x
		cld
		mov	edx, edi
		add	edx, 28h
		mov	ebx, offset32 table_2BB
		mov	ecx, 28h
	@e_541:
		lodsb
		add	al, [ebx]
		inc	ebx
		push	ecx
		mov	ecx, 8
	@e_542:
		rcl	al, 1
		rcl	byte ptr [edi],	1
		inc	edi
		cmp	edi, edx
		jl	short @e_543
		sub	edi, 28h
	@e_543:
		loop	@e_542
		pop	ecx
		loop	@e_541
		mov	esi, edi
		mov	ecx, 14h
		mov	ebx, offset32 table_2BB
	@e_544:
		lodsw
		mul	word ptr [ebx]
		inc	ebx
		inc	ebx
		stosw
		loop	@e_544
		ret
EndProc Encrypt54x
;ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß
; Encrypts out buffer
; Input: EDI - buffer for encrypt params
;
BeginProc _Decrypt54@0
BeginProc Decrypt54
		pushad
		xchg	edi, esi

		push	esi
		push	edi
		add	esi, 28h
		add	edi, 28h
		call	Decrypt54x
		pop	edi
		pop	esi
		call	Decrypt54x
		popad
		ret
EndProc Decrypt54
EndProc _Decrypt54@0
;***
BeginProc Decrypt54x
		cld
		push	esi
		mov	ecx, 14h
		mov	ebx, offset32 table_661
		push	edi
		mov	edi, esi
	@__541:
		lodsw			; ES:EDI->AX
		mul	word ptr [ebx]
		inc	ebx
		inc	ebx
		stosw			; DS:ESI
		loop	@__541
		pop	edi
		pop	esi
		mov	edx, esi
		add	edx, 28h
		mov	ebx, offset32 table_2BB
		mov	ecx, 28h
	@__542:
		push	ecx
		mov	ecx, 8
	@__543:
		rcl	byte ptr [esi],	1
		rcl	al, 1
		inc	esi
		cmp	esi, edx
		jl	short @__544
		sub	esi, 28h
	@__544:
		loop	@__543
		pop	ecx
		sub	al, [ebx]
		inc	ebx
		stosb
		loop	@__542
		ret
EndProc Decrypt54x
_TEXT ENDS
;-----------------------------------------------------
_DATA		SEGMENT PUBLIC 'DATA' USE32
		ASSUME  CS:_TEXT, DS:_DATA
;
PUBLIC		_m_Buf
;-3.00.100
;IFNDEF NOSTRINGS
;		db	"===> Oladd'in Softmare, LMD <==="
;ENDIF
_m_Buf		db	"Warning: do not use HASP-plug, because its protection is easy, "
		db	"mathematical sequence was discovered, and do not try to search "
		db	"any strings in our emu - in the next version we'll change them, "
		db	"because we have sources =)",0dh,0ah,0dh,0ah
;+3.00.100
;--------------------------------------------------------------------
; Internal vars for Decrypt28/Encrypt28
;
table_2BB	db	097h,00fh,089h,085h,06fh,0aah,0d1h,036h
		db	087h,036h,0d9h,0f5h,0dfh,0e9h,0a1h,094h
		db	077h,04ah,029h,055h,04fh,0aeh,071h,069h
                db	067h,0bbh,079h,073h,0bfh,0e7h,041h,005h
		db	057h,0f9h,0c9h,020h,02fh,086h,011h,0b8h
                db	047h,074h,019h,02dh,09fh,079h,0e1h,0d1h
		db	037h,09ch,069h,068h,00fh,0b2h,0b1h,0a2h
                db	027h,0e1h,0b9h,0a2h,07fh,01fh,081h,07ah
		db	017h,0b3h,009h,0ach,0efh,0b1h,051h,0a9h
                db	007h,082h,059h,054h,05fh,059h,021h,07fh
table_661	db	027h,060h,0b9h,080h,08fh,0f4h,031h,0e2h
		db	037h,0afh,069h,03ah,01fh,012h,061h,04fh
		db	047h,0afh,019h,017h,0afh,0a8h,091h,037h
                db	057h,030h,0c9h,086h,03fh,008h,0c1h,08ah
		db	067h,002h,079h,0f9h,0cfh,080h,0f1h,038h
                db	077h,0f5h,029h,0dfh,05fh,062h,021h,032h
		db	087h,0d9h,0d9h,0a7h,0efh,0fch,051h,066h
                db	097h,07eh,089h,0c3h,07fh,0a0h,081h,0c5h
		db	0a7h,0b4h,039h,0a2h,00fh,09dh,0b1h,03fh
                db	0b7h,04bh,0e9h,0b3h,09fh,042h,0e1h,0c4h
;
_DATA ENDS
END