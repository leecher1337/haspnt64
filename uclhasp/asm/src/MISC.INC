;
; support address meteo@null.net
;
;
; Macro definition file 
; Created: 12.07.99 05:32
;
.XLIST
mov_x		MACRO	var, reg
local	@@1
			push	ebp
			call	@@1
	@@1:		pop	ebp
			mov	reg, [ebp+var-@@1]
			pop	ebp
ENDM

push_x		MACRO	Ofset
			push	Ofset
comment #
	local	@@1, @@IP$Rel
			push	eax
			call	@@1
	@@1:
			pop	eax
	@@IP$Rel	=	offset @@1
			add	eax, (offset Ofset - @@IP$Rel)
			xchg	eax, [esp]	; jump to handler
#
ENDM
;
; String encryption macros
;
;
AllocX MACRO arg1,arg2
&arg1	label byte
irpc char,arg2
	db	('&char' xor encryption_byte0) xor encryption_byte1
endm
endm
;
AllocZ MACRO arg1,arg2
&arg1	label byte
irpc char,arg2
	db	('&char' xor encryption_byte3) - encryption_byte2
endm
	db	(0 xor encryption_byte3)  - encryption_byte2
endm
;
.LIST
