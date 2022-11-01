;
; support address meteo@null.net
;
.model tiny, StdCall
.386
;
GLOBAL	_m_NetMemory	: WORD
GLOBAL	_t_Supported	: DWORD
GLOBAL	_heapSupported	: DWORD
GLOBAL	_heapMemory	: DWORD
;
cseg		SEGMENT PUBLIC 'CODE' USE16
		ASSUME  CS:cseg, DS:cseg

;
_t_Supported	dd	?	; Total amount of supported dongles
_heapSupported	dd	?	; Heap for keyTable
_heapMemory	dd	?	; Heap for memoryTable
;
;			 FFFA    FFFB    FFFC    FFFD    FFFE    FFFF
_m_NetMemory	dw	0FFFFh, 0FFFFh, 0FFFFh, 0FFFFh, 0FFFFh, 0FFFEh	; FF00 - MemoHASP
;						 |		 |	; FFFE - NetHASP
;						 |		 |______ indicates HASP type
;						 |______________________ HASP-5,10,25,255,Unlimited
;
cseg ENDS
END