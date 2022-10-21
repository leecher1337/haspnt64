;
; support address meteo@null.net
;
; 
; Invalid opcodes handling routines.
;
.386p
.MODEL FLAT, StdCall
;
		GLOBAL	Call_HASP	: PROC
		GLOBAL	_m_Buf		: BYTE
;
;
include w32.inc
include haspfunc.inc
include haspdef.inc
include misc.inc
;
_TEXT		SEGMENT PUBLIC 'CODE' USE32
		ASSUME  CS:_TEXT
;ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß
;
;
BeginProc _InstallInt06@0
BeginProc InstallInt06
		push	eax
		push	ecx

		mov	ax, ss
		mov	ds, ax
		sidt	qword ptr ds:idt
		mov	eax, ds:idt_base
		mov	cx, [eax+6h*8+6]
		shl	ecx, 10h
		mov	cx, [eax+6h*8]
		cmp	ecx, offset32 Int06h
		je	@inst

		call	SetupInt06
	@inst:
		pop	ecx
		pop	eax
		clc
		ret
EndProc InstallInt06
EndProc _InstallInt06@0
;
;ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß
; Implement invalid opcode handler into IDT
;
;
BeginProc _SetupInt06@0
BeginProc SetupInt06
		push	ds
		push	eax
		push	ebx
		mov	eax, cr0
		push	eax
		and	eax, 0FFFEFFFFh		; WP bit clear
		mov	cr0, eax

		push	ss
		pop	ds
		sgdt	ds:gdt
		sidt	qword ptr ds:idt
		mov	eax, ds:idt_base

                ;storing int 6 handler from IDT
		mov	ebx, [eax+6h*8]		; 0028:2e54
		mov	ds:int6sel_ofs1, ebx
		mov	ebx, [eax+6h*8+4]	; c003:8e:00
		mov	ds:int6ofs2_type, ebx
		mov	bx, word ptr ds:int6ofs2_type+2
		ror	ebx, 10h		; Get offset2 <-high
		mov	bx, word ptr ds:int6sel_ofs1
		mov	ds:int6_old, ebx

                ;storing int E handler from IDT
		mov	ebx, [eax+0Eh*8]		; 0028:2e54
		mov	ds:intEsel_ofs1, ebx
		mov	ebx, [eax+0Eh*8+4]	; c003:8e:00
		mov	ds:intEofs2_type, ebx
		mov	bx, word ptr ds:intEofs2_type+2
		ror	ebx, 10h		; Get offset2 <-high
		mov	bx, word ptr ds:intEsel_ofs1
		mov	ds:intE_old, ebx

                ;setting up int 6 handler
		mov	ebx, offset Int06h
		mov	word ptr [eax+6h*8], bx
		ror	ebx, 10h
		mov	word ptr [eax+6h*8+2], cs
		mov	word ptr [eax+6h*8+6], bx
		mov	byte ptr [eax+6h*8+5], 10001110b ; Gate of Interrupt

                ;setting up int E handler
		mov	ebx, offset Int0Eh
		mov	word ptr [eax+0eh*8], bx
		ror	ebx, 10h
		mov	word ptr [eax+0eh*8+2], cs
		mov	word ptr [eax+0eh*8+6], bx
		mov	byte ptr [eax+0eh*8+5], 10001110b ; Gate of Interrupt

		pop	eax
		mov	cr0, eax

		pop	ebx
		pop	eax
		pop	ds
		ret
EndProc SetupInt06
EndProc _SetupInt06@0
;
BeginProc _RestoreInt06@0
BeginProc RestoreInt06
		push	ds
		push	eax
		push	ebx

		mov	eax, cr0
		push	eax
		and	eax, 0FFFEFFFFh
		mov	cr0, eax

		push	ss
		pop	ds
		sidt	qword ptr ds:idt

		mov	eax, ds:idt_base
		mov	ebx, ds:int6sel_ofs1
		mov	[eax+6h*8], ebx		; 0028:2e54
		mov	ebx, ds:int6ofs2_type
		mov	[eax+6h*8+4], ebx	; c003:8e:00

		mov	ebx, ds:intEsel_ofs1
		mov	[eax+0Eh*8], ebx	; 0028:2e54
		mov	ebx, ds:intEofs2_type
		mov	[eax+0Eh*8+4], ebx	; c003:8e:00

		pop	eax
		mov	cr0, eax

		pop	ebx
                pop	eax
		pop	ds
		ret
EndProc RestoreInt06
EndProc _RestoreInt06@0
;
IFNDEF NOSTRINGS
;		db	"iceman [UCF]",0		; Acknowledgements ;)
ENDIF
;
;ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß
BeginProc Int0Eh
;
IFDEF HASP77
		jmp	short @@jmp2jmp

; ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
		db 'GenuineDevil'	; Genuine Shit =)
		dw 0Eh			; Interrupt #
		dw 201h			; Internal ver
		dd offset CRC_INT_E-offset Int0Eh ; Offset to CRC
; ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
@@jmp2jmp:		jmp	@int0E_boot
;
CRC_E_buffer    db	"iceman [UCF]"
		db	0a6h,009h,049h,034h,000h,000h,000h,000h
                db	049h,008h,073h,001h,000h,000h,000h,000h
                db	000h,000h,000h,000h,000h,000h,064h,0c1h
                db	000h,000h,000h,000h,0ceh,02ah,091h,0c3h
                db	09dh,007h,069h,032h


CRC_INT_E	dd 0BC7EADBEh, 18F4B89Ah, 0E6AAAAFEh, 0FAB56895h, 03DF3690Ch
		dd 0C8D5F9BEh, 0BEFAE6Eh, 02836C8FDh, 00A51715Ah, 081EA7F16h
		dd 09ABA170Bh, 65593A97h, 0DF1DAE90h, 020EACB7Eh, 0D3DFA61Fh
		dd 0767AA6FDh
;
@int0E_boot	LABEL far

ENDIF
		pushad
		push	ds
		push	es

		push	ss		; EquReg
		pop	ds

		mov	eax, cr2

		mov	ecx, offset32 Int0Eh
		mov	edx, offset32 Clear_area	; Bad_Code_Offset

		cmp	eax, ecx
		jb	short @not_at_0E
		cmp	eax, edx
		ja	short @not_at_0E
		cli
		hlt

@not_at_0E:

		pop	es
		pop	ds
		popad

		push	cs:intE_old
		ret
EndProc Int0Eh
; ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
;
intE_old	dd	?
intEsel_ofs1	dd	?
intEofs2_type	dd	?
Clear_area	label	byte
;
IFNDEF NOSTRINGS
;		db	"iceman [UCF]",0
ENDIF
;
;ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß
BeginProc Int06h
;
IFDEF HASP77
		jmp	short @@jmp2jmp2

; ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
aGenuinedevil	db 'GenuineDevil'
		dw 6			; Interrupt #
		dw 301h			; Internal release
		dd offset CRC_INT_6-offset Int06h; CRC???
; ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
@@jmp2jmp2:		jmp  @int06_boot
@@CRC_6_buffer	db	"iceman [UCF]"


;		db	024h - (($-offset Int06h) MOD 024h) dup (0) ; Alignment


                db 0b9h,04ah,01ah,011h,000h,000h,000h,000h
                db 0e9h,00eh,001h,0ebh,000h,000h,000h,000h
                db 000h,000h,000h,000h,000h,000h,0bbh,071h
                db 000h,000h,000h,000h,07ah,012h,052h,0bbh
                db 009h,02eh,02fh,098h
      
CRC_INT_6 	dd 057106F64h, 02A09954Ah, 0E4526E5Bh, 7A9AE221h, 008CBC55Ah
		dd 0DFF13BE0h, 02200C3B6h, 0D9B9B588h, 6B332127h, 0CAE75DE6h
		dd 0AE7E0F34h, 098B49150h, 010BBE0F4h, 2BA7A31Ah, 0126137ECh
		dd 0623C7CC9h

@int06_boot LABEL far
;
ENDIF
		push	ds
		push	es
		push	eax
		push	ecx
		push	edi
		mov	ax, ss
		mov	ds, ax
		mov	es, ax
		ASSUME	ds:_TEXT
		test	Opcode_proceed, 80000000h
		jz	short proceed
		pop	edi
		pop	ecx
		pop	eax
		pop	es
		pop	ds
		iretd
;
;ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß
proceed:
		sldt	word ptr ds:gdt+6
		sidt	qword ptr ds:idt

		mov	ax, [esp+18h]	; SS3 (level3)
		mov	ecx, [esp+14h]	; ESP3 (level 3)
		mov	edi, [esp+1Ch]	; CR3
		call	Bad_Code_Offset	; edi:CR3,
					; ecx:ESP3
					; ax :SS,
					; Return: eax-linear offset to code

		cmp	dword ptr [eax], 100EFFEh
		jnz	short @next_invalid
		cmp	dword ptr [eax+4], 5040302h
		jnz	short @next_invalid
		jmp	short @invalid_valid ; Valid handled invalid opcode
@next_invalid:
		cmp	dword ptr [eax], 0ADDE0B0Fh
		jnz	not_invalid	; Not handled opcode
		cmp	dword ptr [eax+4], 0BEBAEFBEh
		jnz	not_invalid	; Not handled opcode
; ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
@invalid_valid:
		mov	eax, [esp+8]	; SS0
		cmp	eax, 1000000000000000000000000000000b
		jb	not_invalid
		and	eax, 0BFFFFFFFh

		mov	edi, offset encrypt_value
		mov	ecx, t_encrypt_value
		cld
		repne scasb
		jnz	not_invalid
		or	Opcode_proceed, 80000000h
		mov	eax, offset ofs_encrypted
		mov	_edx_save, edx
		mov	edx, offset off_79A7

		not	ecx
		add	ecx, t_encrypt_value
		jmp	dword ptr [eax+ecx*4]
; ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
crypted_1:
		mov	eax, dr7
		push	edx
		mov	edx, [edx]
		mov	[edx], eax
		pop	edx
		sub	eax, eax
		mov	dr7, eax
		push	edx
		mov	edx, [edx+8]
		or	byte ptr [edx],	1
		pop	edx
		jmp	add_eip
; ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
crypted_2:
		push	edx
		mov	edx, [edx]
		mov	eax, [edx]
		pop	edx
		mov	dr7, eax
		push	edx
		mov	edx, [edx+8]
		and	byte ptr [edx],	0FEh
		pop	edx
		jmp	add_eip
; ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
crypted_3:
		mov	eax, ebx
		mov	ecx, [edx+4]
		mov	ecx, [ecx+6]
		mov	edi, [esp+1Ch]
		call	Bad_Code_Offset	; edi:CR3,
					; ecx:ESP3
					; ax :SS,
					; Return: eax-linear offset to code
		xchg	eax, edi
		mov	ecx, [esp+4]
	cry3_loop:
		mov	eax, [edi+1]
		rol	byte ptr [edi],	4
		xor	byte ptr [edi],	0AAh
		inc	edi
		dec	ecx
		jnz	short cry3_loop
		jmp	add_eip
; ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
crypted_4:
		mov	eax, ebx
		mov	ecx, [edx+4]
		mov	ecx, [ecx+6]
		mov	edi, [esp+1Ch]
		call	Bad_Code_Offset	; edi:CR3,
					; ecx:ESP3
					; ax :SS,
					; Return: eax-linear offset to code
		xchg	eax, edi
		mov	ecx, [esp+4]
	cry4_loop:
		rol	byte ptr [edi],	4
		xor	byte ptr [edi],	0AAh
		inc	edi
		dec	ecx
		jnz	short cry4_loop
		mov	eax, [esp]
		mov	[esp+14h], eax
		mov	[esp+18h], esi
		jmp	dont_add_eip
; ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
crypted_5:
		mov	eax, ebx
		mov	ecx, [edx+4]
		mov	ecx, [ecx+6]
		mov	edi, [esp+1Ch]
		call	Bad_Code_Offset	; edi:CR3,
					; ecx:ESP3
					; ax :SS,
					; Return: eax-linear offset to code
		xchg	eax, edi
		mov	ecx, [esp+4]
	cry5_loop:
		xor	byte ptr [edi+ecx-1], 0AAh
		ror	byte ptr [edi+ecx-1], 4
		dec	ecx
		jnz	short cry5_loop
		jmp	add_eip
; ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
crypted_6:
		mov	eax, ebx
		mov	ecx, [edx+4]
		mov	ecx, [ecx+6]
		mov	edi, [esp+1Ch]
		call	Bad_Code_Offset	; edi:CR3,
					; ecx:ESP3
					; ax :SS,
					; Return: eax-linear offset to code
		xchg	eax, edi
		mov	ecx, [esp+4]
	cry6_loop:
		xor	byte ptr [edi+ecx-1], 0AAh
		ror	byte ptr [edi+ecx-1], 4
		dec	ecx
		jnz	short cry6_loop
		mov	eax, [esp]
		mov	[esp+14h], eax
		mov	[esp+18h], esi
		jmp	dont_add_eip
; ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
crypted_7:
		mov	eax, ebx
		mov	ecx, [edx+4]
		mov	ecx, [ecx+6]
		mov	edi, [esp+1Ch]
		call	Bad_Code_Offset	; edi:CR3,
					; ecx:ESP3
					; ax :SS,
					; Return: eax-linear offset to code
		xchg	eax, edi
		sub	eax, eax
		mov	ecx, [esp+4]

	cry7_loop:
		add	al, [edi]
		inc	edi
		dec	ecx
		jnz	short cry7_loop
		push	eax
		mov	eax, esi
		mov	ecx, [esp+4]
		mov	edi, [esp+20h]
		call	Bad_Code_Offset	; edi:CR3,
					; ecx:ESP3
					; ax :SS,
					; Return: eax-linear offset to code
		xchg	eax, edi
		pop	eax
		stosb
		jmp	add_eip
; ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
crypted_8:
		mov	eax, ebx
		mov	ecx, [edx+4]
		mov	ecx, [ecx+6]
		mov	edi, [esp+1Ch]
		call	Bad_Code_Offset	; edi:CR3,
					; ecx:ESP3
					; ax :SS,
					; Return: eax-linear offset to code
		sub	edx, edx
		xchg	eax, edi
		mov	ecx, [esp+4]
	cry8_loop:
		mov	eax, dr0
		cmp	eax, edi
		jnz	short @dr1_8
		mov	eax, dr7
		test	eax, 3
		jz	short @dr1_8
		push	eax
		shr	eax, 10h
		and	eax, 2
		cmp	eax, 2
		pop	eax
		jz	short @dr1_8
		inc	edx
	@dr1_8:
		mov	eax, dr1
		cmp	eax, edi
		jnz	short @dr2_8
		mov	eax, dr7
		test	eax, 0Ch
		jz	short @dr2_8
		push	eax
		shr	eax, 14h
		and	eax, 2
		cmp	eax, 2
		pop	eax
		jz	short @dr2_8
		inc	edx
	@dr2_8:
		mov	eax, dr2
		cmp	eax, edi
		jnz	short @dr3_8
		mov	eax, dr7
		test	eax, 30h
		jz	short @dr3_8
		push	eax
		shr	eax, 18h
		and	eax, 2
		cmp	eax, 2
		pop	eax
		jz	short @dr3_8
		inc	edx
	@dr3_8:
		mov	eax, dr3
		cmp	eax, edi
		jnz	short cry8_quit
		mov	eax, dr7
		test	eax, 0C0h
		jz	short cry8_quit
		push	eax
		shr	eax, 1Ch
		and	eax, 2
		cmp	eax, 2
		pop	eax
		jz	short cry8_quit
		inc	edx
	cry8_quit:
		inc	edi
		dec	ecx
		jnz	cry8_loop
		jmp	EncryptAgain
; ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
crypted_9:
		mov	eax, [edx+4]
		sub	edx, edx
		cmp	byte ptr [eax+5], 0
		jz	EncryptAgain
		mov	eax, cr4
		test	eax, 8
		jz	EncryptAgain
		movzx	edi, dx
		mov	eax, dr0
		cmp	eax, edi
		jnz	short @dr1_9
		mov	eax, dr7
		test	eax, 3
		jz	short @dr1_9
		push	eax
		shr	eax, 10h
		and	eax, 2
		cmp	eax, 2
		pop	eax
		jnz	short @dr1_9
		inc	edx
	@dr1_9:
		mov	eax, dr1
		cmp	eax, edi
		jnz	short @dr2_9
		mov	eax, dr7
		test	eax, 0Ch
		jz	short @dr2_9
		push	eax
		shr	eax, 14h
		and	eax, 2
		cmp	eax, 2
		pop	eax
		jnz	short @dr2_9
		inc	edx
	@dr2_9:
		mov	eax, dr2
		cmp	eax, edi
		jnz	short @dr3_9
		mov	eax, dr7
		test	eax, 30h
		jz	short @dr3_9
		push	eax
		shr	eax, 18h
		and	eax, 2
		cmp	eax, 2
		pop	eax
		jnz	short @dr3_9
		inc	edx
	@dr3_9:
		mov	eax, dr3
		cmp	eax, edi
		jnz	short EncryptAgain
		mov	eax, dr7
		test	eax, 0C0h
		jz	short EncryptAgain
		push	eax
		shr	eax, 1Ch
		and	eax, 2
		cmp	eax, 2
		pop	eax
		jnz	short EncryptAgain
		inc	edx
EncryptAgain:
		mov	_xres, edx
		mov	edx, _edx_save
		and	Opcode_proceed, 7FFFFFFFh
		pop	edi
		pop	ecx
		pop	eax
		mov	eax, _xres
		pop	es
		pop	ds
		or	byte ptr [esp+0Ah], 1
		and	byte ptr [esp+9], 0FEh
		iretd
; ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
crypted_a:
		mov	edi, [esp]
		mov	ecx, [esp+4]
		mov	eax, [esp+8]
                call	dword ptr [esp]
                mov	[esp+4], ecx
                mov	[esp+8], eax		
		jmp	add_eip
; ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
crypted_b:
		mov	eax, [esp]
		cmp	dword ptr [eax], 'hPhW'
		jne	add_eip
;---------------
		push	ebp
		push	eax
		push	eax
		mov	edi, [eax+8]
		mov	esi, [eax+0Ch]
		mov	ebx, [eax+10h]
		mov	edx, [eax+14h]
		mov	ecx, [eax+18h]
		mov	dword ptr [eax+20h], 0
		mov	eax, [eax+1Ch]
		
 		mov	ebp, offset _m_Buf
		mov	[ebp.__EAX],eax
		mov	[ebp.__BH],bh
		mov	[ebp.__ECX],ecx
		mov	[ebp.__EDX],edx
		mov	[ebp.__ESI],esi
    		mov	[ebp.__EDI],edi
						; Store linear address of
		mov	[ebp+40h], eax		; return params at #ReadBlock
		push	fs

		call	Call_HASP

		pop	fs
		pop	ebp			; #EAX
		mov	[ebp+8], edi
		mov	[ebp+0Ch], esi
		mov	[ebp+10h], bx
		mov	[ebp+14h], dx
		mov	[ebp+18h], cx
		mov	[ebp+1Ch], ax
		pop	eax
		pop	ebp

                jmp	short add_eip
; ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
add_eip:	add	dword ptr [esp+14h], 8
dont_add_eip:	mov	edx, _edx_save
		and	Opcode_proceed, 7FFFFFFFh
		pop	edi
		pop	ecx
		pop	eax
		pop	es
		pop	ds
		or	byte ptr [esp+0Ah], 1
		and	byte ptr [esp+9], 0FEh
		iretd
; ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
not_invalid:
		and	Opcode_proceed, 7FFFFFFFh
		pop	edi
		pop	ecx
		pop	eax
		pop	es
		pop	ds
		jmp	ss:int6_old
;--------------------------------------------------------------------
public		idt
public		idt_base
;
int6_old	dd	?
int6sel_ofs1	dd	?
int6ofs2_type	dd	?
idt		dw	?
idt_base	dd	?
buf5		dd	5 dup(?)
gdt		dq	?
encrypt_value	db	1, 2, 3, 4, 5, 6, 7, 8, 9, 0ah, 0bh
t_encrypt_value	=	$-encrypt_value
;
ofs_encrypted	dd	offset32 crypted_1
		dd	offset32 crypted_2
		dd	offset32 crypted_3
		dd	offset32 crypted_4
		dd	offset32 crypted_5
		dd	offset32 crypted_6
		dd	offset32 crypted_7
		dd	offset32 crypted_8
		dd	offset32 crypted_9
		dd	offset32 crypted_a	; Method 0xA (Call VirtCPU32)
		dd	offset32 crypted_b	; Call HASP_API
;
Opcode_proceed	dd	?
_xres		dd	?
		dw	?
_edx_save	dd	?

off_79A7	dd	offset buf5
		dd	offset _xres
		dd	offset Opcode_proceed
;
EndProc Int06h
; ÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛ
;		S u b r	o u t i	n e
; edi:CR3,
; ecx:ESP3
; ax :SS,
; Return: eax-linear offset to code
;
BeginProc Bad_Code_Offset
		push	ecx
		test	edi, 100000000000000000b; Flags - is VM
		jz	short @VM_addr
		movzx	eax, ax
		shl	eax, 4			; EAX -> seg
		jmp	short @quit_vertet
	@VM_addr:
		test	ax, 100b		; selector - local/global
		mov	ecx, dword ptr ds:gdt+2
		movzx	edi, word ptr ds:gdt+6	; Get linear address
		jz	short @local_dt
		ror	eax, 10h
		mov	al, [edi+ecx+7]
		mov	ecx, [edi+ecx+2]
		rol	ecx, 8
		mov	cl, al
		ror	ecx, 8
		rol	eax, 10h
	@local_dt:
		and	ax, 1111111111111000b
		movzx	edi, ax
		mov	eax, [edi+ecx+2]
		rol	eax, 8
		mov	al, [edi+ecx+7]
		ror	eax, 8
	@quit_vertet:
		pop	ecx
		add	eax, ecx
		ret

EndProc Bad_Code_Offset


_TEXT ENDS

END

