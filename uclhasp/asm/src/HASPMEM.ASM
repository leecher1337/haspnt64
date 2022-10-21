;
; support address meteo@null.net
;
.386p
.model Flat, C
;
GLOBAL	m_NetMemory	: WORD
GLOBAL	t_Supported	: DWORD
GLOBAL	heapSupported	: DWORD
GLOBAL	heapMemory	: DWORD
;
_DATA		SEGMENT PUBLIC 'DATA' USE32
		ASSUME  CS:_DATA, DS:_DATA

;
t_Supported	dd	?	; Total amount of supported dongles
heapSupported	dd	?	; Heap for keyTable
heapMemory	dd	?	; Heap for memoryTable
;
;			 FFFA    FFFB    FFFC    FFFD    FFFE    FFFF
m_NetMemory	dw	0FFFFh, 0FFFFh, 0FFFFh, 0FFFFh, 0FFFFh, 0FFFEh	; FF00 - MemoHASP
;						 |		 |	; FFFE - NetHASP
;						 |		 |______ indicates HASP type
;						 |______________________ HASP-5,10,25,255,Unlimited
;
_DATA ENDS
END