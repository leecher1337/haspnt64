;
; support address meteo@null.net
;
; HASP dump header
; 28.05.97 23:56
;
MEMO_BUFFER_SIZE	equ	56
;
; HASP basical services.
;
HASP_INSTALLED		equ	1   ; 01h
HASP_CODE		equ	2   ; 02h
READ_MEMO               equ	3   ; 03h
WRITE_MEMO              equ     4   ; 04h
GET_HASP_STATUS         equ     5   ; 05h
GET_ID_NUM		equ	6   ; 06h
;
UNDOC_0Bh		equ	11  ; 0Bh
UNDOC_0Ch		equ	12  ; 0Ch
;
MEMOHASP_READ_BLOCK	equ	50  ; 32h
MEMOHASP_WRITE_BLOCK	equ	51  ; 33h
;
; A list of NetHASP services.
;
NET_LAST_STATUS		equ	40  ; 28h
NET_GET_HASP_CODE	equ	41  ; 29h
NET_LOGIN		equ	42  ; 2Ah
NET_LOGOUT		equ	43  ; 2Bh
NET_READ_WORD		equ	44  ; 2Ch
NET_WRITE_WORD		equ	45  ; 2Dh
NET_GET_ID_NUMBER	equ	46  ; 2Eh
NET_READ_MEMO_BLOCK	equ	52  ; 34h
NET_WRITE_MEMO_BLOCK    equ	53  ; 35h
SET_SERVER_BY_NAME	equ	96  ; 60h
SET_CONFIG_FILENAME	equ	55h ; 55h
;
; A list of TimeHASP services.
;
TIME_SET_TIME		equ	70  ; 46h
TIME_GET_TIME		equ	71  ; 47h
TIME_SET_DATE		equ	72  ; 48h
TIME_GET_DATE		equ	73  ; 49h
TIME_WRITE_MEMORY	equ	74  ; 4Ah
TIME_READ_MEMORY	equ	75  ; 4Bh
TIME_WRITE_MEMORY_BLOCK	equ	76  ; 4Ch
TIME_READ_MEMORY_BLOCK	equ	77  ; 4Dh
TIME_GET_ID_NUM     	equ	78  ; 4Eh
;
;
OK			equ	0   ;
NET_READ_ERROR		equ	131 ; 83h
NET_WRITE_ERROR		equ	132 ; 84h
;
OTHER_BLOCK_OP_MIN	equ	81  ; 51h
OTHER_BLOCK_OP_MAX	equ	99  ; 63h
;
;
; HASP block services. (compatibility list)
;
MEMOHASP_READ_BLOCK	equ	50
MEMOHASP_WRITE_BLOCK	equ	51
NETHASP_READ_BLOCK	equ	52
NETHASP_WRITE_BLOCK	equ	53
TIMEHASP_WRITE_BLOCK	equ	76
TIMEHASP_READ_BLOCK	equ	77
;
HASPIn_Reg_Struc	STRUC
Reserved01	DD	5 DUP (?)
__AX		DW	?
__BH		DB	?
Reserved06	DB	?
__EAX		DD	?
__BL		DB	?
Reserved07	DB	3 dup (?)
__ECX		DD	?
__EDX		DD	?
__EDI		DD	?
__ESI		DD	?
Reserved08      DD	?
HASPIn_Reg_Struc	ENDS
;
HASPV86_In_Reg_Struc	STRUC
i_reserved_1	DD	?
i_BH            DB	?
i_reserved_11   DB	3 DUP (?)
i_EAX           DD	?
i_BL            DB	?
i_reserved_2    DB	3 DUP (?)
i_ECX           DD	?
i_EDX           DD	?
i_DI            DW	?
i_reserved_13   DW	?
i_ESI           DD	?
i_ES            DW	?
i_reserved_5    DW	?
i_AX            DW	?
i_reserved_6    DW	?
i_MapESAX       DD	?
HASPV86_In_Reg_Struc	ENDS
;
HASPOut_Reg_Struc	STRUC
o_Reserved01	DD	10 DUP (?)
o_EAX		DD	?
o_EBX		DD	?
o_ECX		DD	?
o_EDX		DD	?
HASPOut_Reg_Struc	ENDS
;
HASPV86Out_Reg_Struc	STRUC
v_Reserved01	DD	6 DUP (?)
v_EAX		DD	?
v_EBX		DD	?
v_ECX		DD	?
v_EDX		DD	?
HASPV86Out_Reg_Struc	ENDS
;
Cell		struc
_Pwd1		dd	?
_Pwd2		dd	?
_Memory		dd	?
_Type		dd	?
Cell ends
