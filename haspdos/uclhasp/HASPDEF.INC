;
; support address meteo@null.net
;
;
; Internal emulator definitions
; (c)1998-1999 by MeteO
;
.XLIST
;
; Dongle types
;
HASP3		Equ	0		; Unused
MemoHASP	Equ	1
TimeHASP	Equ	3		; Unused
NetHASP		Equ	4
TimeHASP3	Equ	5		; Unused
;
; Internal variables
;
NOSTRINGS	EQU	0		; If additional string not allowed
;
;CACHEII	EQU	0		; Cache Level II enabled (unused)
;
;DEBUGGING	EQU	0		; Trap messages
;OEM		EQU	0
;
REGISTERED	EQU	0		; Is emu registered
HASP77		EQU	1		; Is CRC_06/0E allowed
;
; DIOC calls
;
HASP_DIOC_CALLHASP	Equ	1
HASP_DIOC_PARAMS	Equ	2
HASP_DIOC_LINPAGELOCK	Equ	4
HASP_DIOC_LINPAGEUNLOCK	Equ	8
HASP_DIOC_GETVER	Equ	10h
HASP_DIOC_RESERVED	Equ	20h
;
; Trial date
; 
day	= 25
month	= 01
year	= 1999
;
IFNDEF OEM
encryption_byte0 = 4
encryption_byte1 = 7
encryption_byte2 = 0Ah
encryption_byte3 = 0EFh
ELSE
encryption_byte0 = 0
encryption_byte1 = 0
encryption_byte2 = 0
encryption_byte3 = 0
ENDIF
;
MAX_RUN		= 5		; Maximal # of runs
MAX_FRIENDLY_NAME	EQU	256
;
.LIST
;
; <EOF>