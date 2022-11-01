; 19.04.98 16:50/11.07.99 23:40
; Hasp'Emu routines. All rights lefted, all lefts righted
; (x)1998-1999 by MeteO (meteo@null.net)
;
.model tiny, StdCall
.386
;
include w32.inc
include haspfunc.inc
include	haspdef.inc
cseg		SEGMENT PUBLIC 'CODE' USE16
		ASSUME  CS:cseg, DS:cseg
;
;
;
; Externals and publics for fn-emu.asm
;
		GLOBAL	Prog_type	: DWORD
		EXTRN	_heapMemory	: DWORD
		EXTRN	_m_NetMemory	: WORD
		EXTRN	GetDongleType	: PROC
		EXTRN	GetOffsetToMem	: PROC
		EXTRN	GetMemSize	: PROC
		GLOBAL	_IsKeySupported@0	: PROC
		GLOBAL	_ReadRegistry@0	: PROC
		GLOBAL	Call_HASP	: PROC
IFDEF DEBUGGING
		GLOBAL	_DumpInRegs@0	: PROC
		GLOBAL	_DumpOutRegs@0	: PROC
ENDIF
;
IFNDEF REGISTERED
		EXTRN	bCRCValues	: DWORD
ENDIF
;

;
;ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß
BeginProc _Call_HASP@0
BeginProc Call_HASP
IFDEF DEBUGGING
		pushad
		call	_DumpInRegs@0
		popad
ENDIF
		call	_IsKeySupported@0
		jnc	@scan_fn
		call	_ReadRegistry@0
                jc	INVALID_KEY		; No such Key
		
	@scan_fn:
		std
		movzx   ebx, [ebp.i_BH]		; Func #
		mov     ecx, t_KeyTbl		; Length of supported fn# table
		mov	esi, offset32 KeyTbl + t_KeyTbl-1
	@@loopfn:
		lodsb				; ds:esi -> ax
		cmp	al, bl
		je	@@jump			; Has found
		loop	@@loopfn
		jmp	INVALID_SERVICE		; No such service
	@@jump:
		lea	ebx, KeyHandle
		mov	ebx, [ebx+ecx*4]

		call	bx			; Call Emu-API
IFDEF DEBUGGING
		pushad
		call	_DumpOutRegs@0
		popad
ENDIF
		ret
EndProc Call_HASP
EndProc _Call_HASP@0
;
;ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß
; Fn1	: IsHasp(): - HaspFound
; Params: None
; Return: AX : 0 - Hasp not found
;	       1 - Hasp found
;
BeginProc _HASP_INSTALLED
			mov	eax, 1
		@ebcx:
			xor	ebx, ebx
		@res:
			xor	ecx, ecx
			mov	edx, [ebp.i_EDX]	; Bugfix
			clc
	ret
EndProc _HASP_INSTALLED
;
;ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß
; Fn2	: HaspSeed(Seed)
; Params: AX : SeedCode
; Return: AX : 1st return code
;         BX : 2nd return code
;         CX : 3rd return code
;         DX : 4th return code
;
BeginProc _NET_LOGIN
BeginProc _HASP_CODE
			cmp	word ptr [ebp.i_ECX], 0
			jne	f2
			cmp	word ptr [ebp.i_EDX], 0
			jne	f2

			mov	eax, 0BB58h	; Special case for
			mov	ebx, 0E11Fh	; pwd1:pwd2=0:0
			mov	ecx, 07A86h
			mov	edx, 0DCD9h
			ret
		f2:
			cmp	word ptr [ebp.i_ECX], 5
			jne	f22
			cmp	word ptr [ebp.i_EDX], 5
			jne	f22

			xor	eax, eax	; Special case for
			xor	ebx, ebx	; pwd1:pwd2=5:5
			xor	ecx, ecx
			cdq                     ; the same as xor edx, edx
			ret

		f22:
			push	ebp
			movzx	eax, word ptr [ebp.i_EAX]
			push	word ptr [ebp.i_EDX]; Just get long pwd
			push	word ptr [ebp.i_ECX]
			pop	ebp
;---------------------------------------------------------------------------
			xor	ebp, 9071966h
			call	Gen16bit
			push	bx
			call	Gen16bit
			push	bx
			call	Gen16bit
			push	bx
			call	Gen16bit
			mov	dx, bx
			pop	cx
			pop	bx
			pop	ax
IFNDEF REGISTERED
                        add	eax, [bCRCValues]
ENDIF
			pop	ebp
		ret
EndProc _HASP_CODE
EndProc _NET_LOGIN
;
BeginProc Gen16bit
;------------------------------------------------------------------------
;
;In:	AX  - seed code
;	EBP - password
;Out:	AX  - new seed code
;	BX  - 16 bit of ret value
;destroys: cx,dx,edi
			mov	dx,  16
	@@bit_loop:
					; generate next seed code
			imul	ax, 1989h
			add	ax, 5
                                	; calc 1 bit:
			push	ax
			mov	cl, ah
			shr	cl, 1
			and	cl, 7
			mov	ch, cl
			shl	cl, 1
			add	cl, ch	; cl = (ah/2) * 3
			mov	edi, ebp
			shr	edi, cl
			mov	cx,di	;cl[2:0] - needed bits
			dec	cl
			and	cl, 7
			or	ah, 80h
			shr	cl, 1
			jnc	@@over
			not	ah
	@@over:
			add	cl, 5
			shr	ah, cl
			rcl	bx, 1
			pop	ax
			dec	dx
			jnz	@@bit_loop

			xchg	bh, bl
		ret
EndProc Gen16bit
;
;
BeginProc _TIME_READ_MEMO
BeginProc _NET_READ_MEMO
BeginProc _READ_MEMO
;
;ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß
; Fn3	: ReadMemo(Memo)
; Params: DI : Memory Addr
; Return: BX : Data
;	  CX : Status
;
			mov	edi, [ebp.i_EDI]
			and	edi, 0FFFFh	; Make word

			mov	eax, Prog_type
			call	GetOffsetToMem	; ebx contains offset

                        mov	eax, ds:[_heapMemory]

			lea	ebx, [eax+ebx]

                        mov	eax, edi	; compatibility
                        test	edi, 0f000h
                        jnz	@netread_fffx
;+2.50.706 add-on
			mov	eax, Prog_type	; Memory range checking
			call	GetDongleType
			call	GetMemSize
			cmp	edi, eax	; Memory range exceeds limit
			ja	@fault_emu
;+2.50.706
		@xread:
			shl	edi, 1		; Word access (DI*2)
		@zread:
			movzx	ebx, word ptr [ebx+edi+4]; return fake data
IFNDEF REGISTERED
			add	ebx, [bCRCValues+5]
ENDIF
		@edi:
			mov	eax, [ebp.i_EDI]; Restore EAX
			mov	edi, eax	; Restore EDI
			jmp	@res		; HASP works properly

		@netread_fffx:
			cmp	edi, 0FFF9h	; ID high
			jg	@not_ID
		@pret:
                	cmp	edi, 0FFF8h	; ID low
                        jl	@fault_emu

			add	di, 8
                        shl	di, 1		;
                        sub	edi, 4		; S/n
			jmp	short @zread
		@not_ID:
			lea	ebx, _m_NetMemory
			add	edi, 8-4
			and	edi, 0FFFFh
			jmp	short @xread
                        			; Case of emu checking
		@fault_emu:
                	mov	ecx, -2		; illegal address
                        ret
EndProc _READ_MEMO
EndProc _NET_READ_MEMO
EndProc _TIME_READ_MEMO
;
BeginProc _TIME_WRITE_MEMO
BeginProc _NET_WRITE_MEMO
BeginProc _WRITE_MEMO
;
;ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß
; Fn4	: WriteMemo(Addr,Value)
; Params: DI : Memory addr
;	  SI : Memory data
; Return: CX : Status
;
			mov	edi, [ebp.i_EDI]
			mov	esi, [ebp.i_ESI]
			mov	eax, Prog_type
                        call	GetOffsetToMem	; at ebx offset

			mov	eax, ds:[_heapMemory]
			lea	ebx, [eax+ebx]

			and	edi, 0ffffh
;+2.50.706 add-on
			mov	eax, Prog_type	; Memory range checking
			call	GetDongleType
			call	GetMemSize
			cmp	edi, eax	; Memory range exceeds limit
			ja	@fault_emu
;+2.50.706 add-on
                        mov	eax, edi	; compatibility
			shl	edi, 1		; Word access (DI*2)
			cmp	eax, 0		; Unable to write Hidden words!
			jge	is_ok
                        jmp	@fault_emu	; Illegal address
		is_ok:
			mov	word ptr [ebx+edi+4], si; return fake data
			jmp	@edi		; HASP works properly
EndProc _WRITE_MEMO
EndProc _NET_WRITE_MEMO
EndProc _TIME_WRITE_MEMO
;ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß
; Fn5	: HaspStatus()
; Params: None
; Return: AX : Memory size
;	  BX : Hasp type
;	  CX : Actual LPT_num
;
;
BeginProc _GET_HASP_STATUS

			mov	eax, Prog_type
			call	GetDongleType
			mov	ebx, [ebp.i_ECX]	; Pwd #1
			and	ebx, 1
			mov	ecx, 66h		; Actual LPT num
			xor	edx, edx
			clc
		ret
EndProc _GET_HASP_STATUS
;
;ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß
; Fn6	: GetId()
; Params: None
; Return: AX : IDLow
;	  BX : IDHigh
;	  CX : Status
;

BeginProc _TIME_GET_ID_NUM
BeginProc _NET_GET_ID_NUM
BeginProc _GET_ID_NUM
			cmp	word ptr [ebp.i_ECX], 5
			jne	f5
			cmp	word ptr [ebp.i_EDX], 5
			jne	f5

			xor	eax, eax	; Special case for
			xor	ebx, ebx	; pwd1:pwd2=5:5
			xor	ecx, ecx
			cdq                     ; the same as xor edx, edx
			ret
		f5:
			mov	eax, Prog_type
                        call	GetOffsetToMem	; at ebx offset

                        mov	eax, ds:[_heapMemory]
			lea	ebx, [eax+ebx]
			movzx   eax, word ptr [ebx+2]	; Return s/n
			movzx   ebx, word ptr [ebx]
IFNDEF REGISTERED
                        add	ebx, [bCRCValues+9]
ENDIF
			jmp	@res			; HASP works properly

EndProc _GET_ID_NUM
EndProc _NET_GET_ID_NUM
EndProc _TIME_GET_ID_NUM
;ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß
; Fn0B: Inport (Undocumented)
; Params: DI : Port
; Return: AX : Value
;
;
BeginProc _UNDOC_0Bh
			xor	eax, eax
                        xor	ebx, ebx
                        xor	ecx, ecx
                        mov	edx, 0246h		; ???
			jmp	@res
EndProc _UNDOC_0Bh
;ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß
; Fn0C: Outport (Undocumented)
; Params: DI:Port
;	  SI:Value
; Return: None
;
;
BeginProc _UNDOC_0Ch
			xor	eax, eax
                        xor	ebx, ebx
                        xor	ecx, ecx
                        mov	edx, 0246h		; ???
			jmp	@res
EndProc _UNDOC_0Ch
;
;ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß
; Fn32  : ReadBlock(*ptr, StartAddr, len):
; Params: DI : Mem.start addr.
;	  SI : Block length
;	  ES : Bufer seg.
;	  AX : Bufer offs.
; Return: CX : Status
;
BeginProc _READ_MEMO_BLOCK
BeginProc _NET_READ_MEMO_BLOCK
BeginProc _TIME_READ_MEMO_BLOCK
                        cld
			mov	eax, Prog_type
                        call	GetOffsetToMem	; at ebx offset

                        mov	esi, ds:[_heapMemory]
			lea	esi, [esi+ebx]
			movzx   ecx, word ptr [ebp.i_ESI]
			movzx	edi, word ptr [ebp.i_EDI]
			test	edi, 0F000h
			jnz	@network_read

			lea	edi, [edi*2+4]
		@_read:
			add	esi, edi
;
; Removed in case of Network read error
;       		add	esi, [ebp.i_EDI]
;       		add	esi, [ebp.i_EDI]	; set destination
;       		add	esi, 4			; S/n
;
			push	es
			mov	es,  [ebp.i_ES]
                        mov     edi, [ebp.i_AX]         ; set source addr
			repe	movsw			; start to store ES:DI<-DS:SI
			pop	es
			jmp	@res
		@network_read:
			cmp	edi, 0FFF9h	; ID high
			jg	@not_block_ID

                	cmp	edi, 0FFF8h	; Bad address
                        jl	@fault_emu

			add	di, 8
                        shl	di, 1		;
                        jmp	short @_read

		@not_block_ID:

			add	edi, 8
			lea	esi, _m_NetMemory
			jmp	short @_read

EndProc _TIME_READ_MEMO_BLOCK
EndProc _NET_READ_MEMO_BLOCK
EndProc _READ_MEMO_BLOCK
;
;ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß
; Fn33  : WriteBlock(*ptr, StartAddr, len):
; Params: DI : Mem.start addr.
;	  SI : Block length
;	  ES : Bufer seg.
;	  AX : Bufer offs.
; Return: CX : Status
;
BeginProc _WRITE_MEMO_BLOCK
BeginProc _NET_WRITE_MEMO_BLOCK
BeginProc _TIME_WRITE_MEMO_BLOCK
                        cld
			mov	eax, Prog_type
                        call	GetOffsetToMem	; at ebx offset

			mov	edi, ds:[_heapMemory]
			lea	edi, [edi+ebx]
                        add     edi, [ebp.i_EDI]
                        add     edi, [ebp.i_EDI]        ; set destination addr
			add	edi, 4			; s/n
                        mov     esi, [ebp.i_AX]          ; set source addr
                        movzx   ecx, word ptr [ebp.i_ESI]; rep counter
			push	es
			mov	es,  [ebp.i_es]

                        repe    movsw			; start to store ES:DI<-DS:SI
			pop	es
			jmp	@res
EndProc _TIME_WRITE_MEMO_BLOCK
EndProc _NET_WRITE_MEMO_BLOCK
EndProc _WRITE_MEMO_BLOCK
;
BeginProc _NET_LAST_STATUS
BeginProc _NET_LOGOUT
                        xor	eax, eax	; Return OK
			jmp	@ebcx
EndProc _NET_LOGOUT
EndProc _NET_LAST_STATUS

BeginProc _TIME_GET_TIME
BeginProc _TIME_SET_TIME
BeginProc _TIME_GET_DATE
BeginProc _TIME_SET_DATE

BeginProc INVALID_SERVICE
			xor	eax, eax
			cdq			; the same as xor edx, edx
			xor	ebx, ebx
			mov	ecx, -999	; Not supported
			ret
EndProc INVALID_SERVICE

EndProc _TIME_SET_DATE
EndProc _TIME_GET_DATE
EndProc _TIME_SET_TIME
EndProc _TIME_GET_TIME

BeginProc INVALID_KEY
			xor	eax, eax
			cdq
			xor	ebx, ebx
			xor	ecx, ecx
			mov	ecx, -3		; No such key

			cmp	byte ptr [ebp.i_BH], HASP_INSTALLED
			jne	@nk2		; Preserve special case if
			mov	eax, 1		; key not found

		@nk2:
			ret
EndProc INVALID_KEY
;
;
;
; Values of supported functions
;
; On HASP call HASP_INSTALLED (BH=1) scan Keyhandle table for proper handler
; if found call item KeyHandle [HASPfunction]
;
KeyTbl		db	HASP_INSTALLED,HASP_CODE
		db	READ_MEMO,WRITE_MEMO
		db	GET_HASP_STATUS,GET_ID_NUM
                db	MEMOHASP_READ_BLOCK, MEMOHASP_WRITE_BLOCK
		db	UNDOC_0Bh, UNDOC_0Ch
;		db	TIME_SET_TIME,TIME_GET_TIME
;		db	TIME_SET_DATE,TIME_GET_DATE
;		db	TIME_WRITE_MEMORY, TIME_READ_MEMORY
;		db	TIME_WRITE_MEMORY_BLOCK, TIME_READ_MEMORY_BLOCK
;		db	TIME_GET_ID_NUM
t_KeyTbl	Equ	$-KeyTbl	; Size
;
KeyHandle	dd	offset32 INVALID_SERVICE
		dd	offset32 _HASP_INSTALLED, offset32 _HASP_CODE
		dd	offset32 _READ_MEMO, offset32 _WRITE_MEMO
		dd	offset32 _GET_HASP_STATUS, offset32 _GET_ID_NUM
                dd	offset32 _READ_MEMO_BLOCK, offset32 _WRITE_MEMO_BLOCK
		dd	offset32 _UNDOC_0Bh, offset32 _UNDOC_0Ch
;
;		dd	offset32 _TIME_SET_TIME, offset32 _TIME_GET_TIME
;		dd	offset32 _TIME_SET_DATE, offset32 _TIME_GET_DATE
;		dd	offset32 _TIME_WRITE_MEMO, offset32 _TIME_READ_MEMO
;		dd	offset32 _TIME_WRITE_MEMO_BLOCK, offset32 _TIME_READ_MEMO_BLOCK
;		dd	offset32 _TIME_GET_ID_NUM
;--------------------------------------------------------------------
;
Prog_type	dd	?
;
cseg ENDS
END
