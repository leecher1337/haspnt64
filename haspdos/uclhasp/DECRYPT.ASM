;
; support address meteo@null.net
;

; 06.08.1999 20:39
;
; Decrypt/Call_HASP routines
;

.model tiny, StdCall
.386
;
include w32.inc
include haspfunc.inc
;
cseg		SEGMENT PUBLIC 'CODE' USE16
		ASSUME  CS:cseg,DS:cseg
;
		EXTRN	Call_HASP	:PROC
        
BeginProc _ReadRegistry@0
BeginProc  ReadRegistry
        stc
        ret
EndProc ReadRegistry
EndProc _ReadRegistry@0

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
		movzx	edi, bp
		mov	ebp, edi
		call	Decrypt28
		push	edi
		call	Call_HASP
		pop	edi
		mov     ss:[edi.v_EAX], eax	; Return values
		mov     ss:[edi.v_EBX], ebx	; Return values
		mov     ss:[edi.v_ECX], ecx	; Return values
		mov     ss:[edi.v_EDX], edx	; Return values
		call	Encrypt28
		popad
	ret
EndProc Buffer0x28
EndProc _Buffer0x28@0
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
		mov	ax, ss:[esi+edi]
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
		mov	ss:[esi+edi], ax
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
		mov	ax, ss:[esi+edi]
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
		mov	ss:[esi+edi], ax
		add	counter, 2
		mov	ax, Final_counter
		cmp	ax, counter
		jnz	@decr_init

		popad
		ret

EndProc Decrypt28
EndProc _Decrypt28@0

;-----------------------------------------------------
cseg ENDS
END