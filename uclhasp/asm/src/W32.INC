;
; support address meteo@null.net
;
;
; VWin replace macros
;
.XLIST
;
NULL                    EQU     00h
CRLF                    EQU     0Dh,0Ah
EOL                     EQU     00h
TRUE                    EQU     00000001h
FALSE                   EQU     00000000h
;
;
;
DEVICE_INIT		EQU	0001h
INIT_COMPLETE		EQU	0002H
CREATE_VM               EQU     0007h
VM_CRITICAL_INIT        EQU     0008h
VM_INIT                 EQU     0009h
VM_TERMINATE            EQU     000Ah
DESTROY_VM              EQU     000Ch
SYS_DYNAMIC_DEVICE_INIT EQU     001Bh
SYS_DYNAMIC_DEVICE_EXIT EQU     001Ch
CREATE_THREAD           EQU     001Dh
TERMINATE_THREAD        EQU     001Fh
THREAD_INIT             EQU     001Eh
W32_DEVICEIOCONTROL	EQU	0023H
;
; ERRORS
;
ERROR_SUCCESS           EQU     0
VXD_SUCCESS		EQU	1
;
; VMStat
;
VMStat_PM_Exec          EQU     20h
VMStat_PM_App           EQU     40h
;
UNDEFINED_INIT_ORDER	EQU	080000000H
UNDEFINED_DEVICE_ID	EQU	00000H
;
; DIOC calls
DIOC_GETVERSION		EQU	0H
DIOC_OPEN		EQU	DIOC_GETVERSION
DIOC_CLOSEHANDLE	EQU	-1
;
;
VxD_Desc_Block STRUC
DDB_Next		DD	?
DDB_SDK_Version		DW	400h
DDB_Req_Device_Number	DW	UNDEFINED_DEVICE_ID
DDB_Dev_Major_Version	DB	0
DDB_Dev_Minor_Version	DB	0
DDB_Flags		DW	0
DDB_Name		DB	'       '
DDB_Init_Order		DD	UNDEFINED_INIT_ORDER
DDB_Control_Proc	DD	?
DDB_V86_API_Proc	DD	0
DDB_PM_API_Proc		DD	0
DDB_V86_API_CSIP	DD	0
DDB_PM_API_CSIP		DD	0
DDB_Reference_Data	DD	?
DDB_Service_Table_Ptr	DD	0
DDB_Service_Table_Size	DD	0
DDB_Win32_Service_Table	DD	0
DDB_Prev		DB	'Prev'
DDB_Size		DD	size VxD_Desc_Block
DDB_Reserved1		DB	'Rsv1'
DDB_Reserved2		DB	'Rsv2'
DDB_Reserved3		DB	'Rsv3'
VxD_Desc_Block ENDS
;
Client_Reg_Struc        STRUC
  Client_EDI            DD      ?
  Client_ESI            DD      ?
  Client_EBP            DD      ?
  Client_res0           DD      ?
  Client_EBX            DD      ?
  Client_EDX            DD      ?
  Client_ECX            DD      ?
  Client_EAX            DD      ?
  Client_Error          DD      ?
  Client_EIP            DD      ?
  Client_CS             DW      ?
  Client_res1           DW      ?
  Client_EFlags         DD      ?
  Client_ESP            DD      ?
  Client_SS             DW      ?
  Client_res2           DW      ?
  Client_ES             DW      ?
  Client_res3           DW      ?
  Client_DS             DW      ?
  Client_res4           DW      ?
  Client_FS             DW      ?
  Client_res5           DW      ?
  Client_GS             DW      ?
  Client_res6           DW      ?
  Client_Alt_EIP        DD      ?
  Client_Alt_CS         DW      ?
  Client_res7           DW      ?
  Client_Alt_EFlags     DD      ?
  Client_Alt_ESP        DD      ?
  Client_Alt_SS         DW      ?
  Client_res8           DW      ?
  Client_Alt_ES         DW      ?
  Client_res9           DW      ?
  Client_Alt_DS         DW      ?
  Client_res10          DW      ?
  Client_Alt_FS         DW      ?
  Client_res11          DW      ?
  Client_Alt_GS         DW      ?
  Client_res12          DW      ?
Client_Reg_Struc        ENDS
;

DIOCParams STRUC
Internal1		DD	?
VMHandle		DD	?
Internal2		DD	?
dwIoControlCode		DD	?
lpvInBuffer		DD	?
cbInBuffer		DD	?
lpvOutBuffer		DD	?
cbOutBuffer		DD	?
lpcbBytesReturned	DD	?
lpoOverlapped		DD	?
hDevice			DD	?
tagProcess		DD	?
DIOCParams ENDS
;
cb_s                    STRUC
  CB_VM_Status          dd      ?       ; VM status; see below
  CB_High_Linear        dd      ?       ; base linear address; see below
  CB_Client_Pointer     dd      ?       ; see below
  CB_VMID               dd      ?       ; virtual machine ID
  CB_Signature          dd      ?       ; see below
cb_s                    ENDS

tcb_s                   STRUC
  TCB_Flags             DD      ?
  TCB_Reserved1         DD      ?
  TCB_Reserved2         DD      ?
  TCB_Signature         DD      ?
  TCB_ClientPtr         DD      ?
  TCB_VMHandle          DD      ?
  TCB_ThreadId          DW      ?
  TCB_PMLockOrigSS      DW      ?
  TCB_PMLockOrigESP     DD      ?
  TCB_PMLockOrigEIP     DD      ?
  TCB_PMLockStackCount  DD      ?
  TCB_PMLockOrigCS      DW      ?
  TCB_PMPSPSelector     DW      ?
  TCB_ThreadType        DD      ?
  TCB_pad1              DW      ?
  TCB_pad2              DB      ?
  TCB_extErrLocus       DB      ?
  TCB_extErr            DW      ?
  TCB_extErrAction      DB      ?
  TCB_extErrClass       DB      ?
  TCB_extErrPtr         DD      ?
tcb_s                   ENDS

; Some registry stuff
REG_SZ			EQU	0001H
REG_BINARY		EQU	0003H
;
HKEY_LOCAL_MACHINE      EQU     80000002h
;
; Егор стуфф
;
NO_ERROR			EQU	0
ERROR_INVALID_FUNCTION		EQU	1
ERROR_FILE_NOT_FOUND		EQU	2
ERROR_PATH_NOT_FOUND		EQU	3
ERROR_TOO_MANY_OPEN_FILES	EQU	4
ERROR_ACCESS_DENIED		EQU	5
ERROR_INVALID_HANDLE		EQU	6
ERROR_ARENA_TRASHED		EQU	7
ERROR_NOT_ENOUGH_MEMORY		EQU	8
ERROR_INVALID_BLOCK		EQU	9
ERROR_BAD_ENVIRONMENT		EQU	10
ERROR_BAD_FORMAT		EQU	11
ERROR_INVALID_ACCESS		EQU	12
ERROR_INVALID_DATA		EQU	13
ERROR_INVALID_DRIVE		EQU	15
ERROR_CURRENT_DIRECTORY		EQU	16
ERROR_NOT_SAME_DEVICE		EQU	17
ERROR_NO_MORE_FILES		EQU	18
ERROR_WRITE_PROTECT		EQU	19
ERROR_BAD_UNIT			EQU	20
ERROR_NOT_READY			EQU	21
ERROR_BAD_COMMAND		EQU	22
ERROR_CRC			EQU	23
ERROR_BAD_LENGTH		EQU	24
ERROR_SEEK	EQU	25
ERROR_NOT_DOS_DISK	EQU	26
ERROR_SECTOR_NOT_FOUND	EQU	27
ERROR_OUT_OF_PAPER	EQU	28
ERROR_WRITE_FAULT	EQU	29
ERROR_READ_FAULT	EQU	30
ERROR_GEN_FAILURE	EQU	31
ERROR_SHARING_VIOLATION	EQU	32
ERROR_LOCK_VIOLATION	EQU	33
ERROR_WRONG_DISK	EQU	34
ERROR_FCB_UNAVAILABLE	EQU	35
ERROR_SHARING_BUFFER_EXCEEDED	EQU	36
ERROR_CODE_PAGE_MISMATCHED	EQU	37
ERROR_HANDLE_EOF	EQU	38
ERROR_HANDLE_DISK_FULL	EQU	39
error_volume_locked	EQU	40
error_invalid_volume_lock	EQU	41
error_volume_lock_failed	EQU	42
ERROR_NOT_SUPPORTED	EQU	50
ERROR_NET_ACCESS_DENIED	EQU	65
ERROR_BAD_NET_NAME	EQU	67
ERROR_FILE_EXISTS	EQU	80
ERROR_DUP_FCB	EQU	81
ERROR_CANNOT_MAKE	EQU	82
ERROR_FAIL_I24	EQU	83
ERROR_OUT_OF_STRUCTURES	EQU	84
ERROR_ALREADY_ASSIGNED	EQU	85
ERROR_INVALID_PASSWORD	EQU	86
ERROR_INVALID_PARAMETER	EQU	87
ERROR_NET_WRITE_FAULT	EQU	88
ERROR_SYS_COMP_NOT_LOADED	EQU	90
ERROR_BUFFER_OVERFLOW	EQU	111
ERROR_CALL_NOT_IMPLEMENTED	EQU	120
ERROR_BAD_PIPE	EQU	230
ERROR_PIPE_BUSY	EQU	231
ERROR_PIPE_NOT_CONNECTED	EQU	233
ERROR_MORE_DATA	EQU	234
ERROR_IO_PENDING	EQU	600
ERROR_TIMEOUT	EQU	601
ERROR_PIPE_CLOSED	EQU	602
ERROR_PIPE_LISTENING	EQU	603
ERROR_TOO_MANY_INSTANCES	EQU	604
ERROR_INVALID_PROVIDER	EQU	605
ERROR_VOLUME_EXISTS	EQU	606
ERROR_VOLUME_HARD_ERROR	EQU	607
ERROR_VOLUME_UNRECOGNIZED	EQU	608
ERROR_LN_GENERIC_NOT_SUP	EQU	7100H
ERROR_LN_FIND_CLOSE_NOT_SUP	EQU	7200H
ERROR_I24_WRITE_PROTECT	EQU	0
ERROR_I24_BAD_UNIT	EQU	1
ERROR_I24_NOT_READY	EQU	2
ERROR_I24_BAD_COMMAND	EQU	3
ERROR_I24_CRC	EQU	4
ERROR_I24_BAD_LENGTH	EQU	5
ERROR_I24_SEEK	EQU	6
ERROR_I24_NOT_DOS_DISK	EQU	7
ERROR_I24_SECTOR_NOT_FOUND	EQU	8
ERROR_I24_OUT_OF_PAPER	EQU	9
ERROR_I24_WRITE_FAULT	EQU	0AH
ERROR_I24_READ_FAULT	EQU	0BH
ERROR_I24_GEN_FAILURE	EQU	0CH
ERROR_I24_WRONG_DISK	EQU	0FH
ALLOWED_FAIL	EQU	08H
ALLOWED_RETRY	EQU	10H
ALLOWED_IGNORE	EQU	20H
I24_OPERATION	EQU	01H
I24_AREA	EQU	06H
I24_CLASS	EQU	80H
ERRCLASS_OUTRES	EQU	1
ERRCLASS_TEMPSIT	EQU	2
ERRCLASS_AUTH	EQU	3
ERRCLASS_INTRN	EQU	4
ERRCLASS_HRDFAIL	EQU	5
ERRCLASS_SYSFAIL	EQU	6
ERRCLASS_APPERR	EQU	7
ERRCLASS_NOTFND	EQU	8
ERRCLASS_BADFMT	EQU	9
ERRCLASS_LOCKED	EQU	10
ERRCLASS_MEDIA	EQU	11
ERRCLASS_ALREADY	EQU	12
ERRCLASS_UNK	EQU	13
ERRACT_RETRY	EQU	1
ERRACT_DLYRET	EQU	2
ERRACT_USER	EQU	3
ERRACT_ABORT	EQU	4
ERRACT_PANIC	EQU	5
ERRACT_IGNORE	EQU	6
ERRACT_INTRET	EQU	7
ERRLOC_UNK	EQU	1
ERRLOC_DISK	EQU	2
ERRLOC_NET	EQU	3
ERRLOC_SERDEV	EQU	4
ERRLOC_MEM	EQU	5
NO_RESPONSE	EQU	80H
SEEK_FAILED	EQU	40H
CNTRLR_FAILED	EQU	20H
DATA_ERROR	EQU	10H
DMA_FAILURE	EQU	08H
SEC_NOT_FOUND	EQU	04H
BAD_ADDRESS	EQU	02H
BAD_COMMAND	EQU	01H
ERROR_UNCERTAIN_MEDIA	EQU	163
ERROR_DISK_FULL	EQU	112
;
; Some cool memory allocation stuff
;

HEAPZEROINIT            EQU     00000001h
HEAPZEROREINIT          EQU     00000002h
HEAPNOCOPY              EQU     00000004h
HEAPLOCKEDIFDP          EQU     00000100h
HEAPSWAP                EQU     00000200h
HEAPINIT                EQU     00000400h
HEAPCLEAN               EQU     00000800h

; Values for charSet passed to character conversion routines

BCS_WANSI               EQU     0       ; Use Windows ANSI set
BCS_OEM                 EQU     1       ; Use current OEM character set
BCS_UNI                 EQU     2       ; Use UNICODE character set

; IFS Function IDs passed to IFSMgr_CallProvider

IFSFN_READ              EQU     0       ; Read a file
IFSFN_WRITE             EQU     1       ; Write a file
IFSFN_CLOSE             EQU     11      ; Close handle
IFSFN_OPEN              EQU     36      ; Open a file
IFSFN_RENAME            EQU     37      ; Rename

ioreq                   STRUC
  ir_length             DD      ?       ; length of user buffer (eCX)
  ir_flags              DB      ?       ; misc. status flags (AL)
  ir_user               DB      ?       ; user ID for this request
  ir_sfn                DW      ?       ; System File Number of file handle
  ir_pid                DD      ?       ; process ID of requesting task
  ir_ppath              DD      ?       ; unicode pathname
  ir_aux1               DD      ?       ; secondary user data buffer (CurDTA)
  ir_data               DD      ?       ; ptr to user data buffer (DS:eDX)
  ir_options            DW      ?       ; request handling options
  ir_error              DW      ?       ; error code (0 if OK)
  ir_rh                 DD      ?       ; resource handle
  ir_fh                 DD      ?       ; file (or find) handle
  ir_pos                DD      ?       ; file position for request
  ir_aux2               DD      ?       ; misc. extra API parameters
  ir_aux3               DD      ?       ; misc. extra API parameters
  ir_pev                DD      ?       ; ptr to IFSMgr event for async requests
  ir_fsd                DB      40h DUP (?) ; Provider work space
ioreq                   ENDS

;
; Ring0 File I/O stuff
;
R0_OPENCREATFILE        EQU     0D500h          ; Open/Create a file
R0_READFILE             EQU     0D600h          ; Read file
R0_WRITEFILE            EQU     0D601h          ; Write to a file, no context
R0_CLOSEFILE            EQU     0D700h          ; Close file
R0_GETFILESIZE          EQU     0D800h          ; Get file size

;
; Сервисы ---------------------------------------------------------------------
;

Get_Cur_VM_Handle                       EQU     00010001h
Get_Sys_VM_Handle                       EQU     00010003h
Allocate_PM_Call_Back                   EQU     0001000Ch
Call_When_VM_Returns                    EQU     0001000Dh
Hook_V86_Int_Chain                      EQU     00010041h
Get_PM_Int_Vector                       EQU     00010044h
Set_PM_Int_Vector                       EQU     00010045h
Simulate_Iret                           EQU     00010047h
Simulate_Far_Call                       EQU     00010048h
Simulate_Far_Jmp                        EQU     00010049h
Simulate_Far_Ret_N                      EQU     0001004Bh
Simulate_Push                           EQU     0001004Dh
LinPageLock				EQU	00010063h
LinPageUnLock				EQU	00010064h
Begin_Nest_Exec                         EQU     00010083h
Exec_Int                                EQU     00010084h
End_Nest_Exec                           EQU     00010086h
Exec_VxD_Int				EQU	0001008Fh
Hook_Device_Service                     EQU     00010090h
Get_Cur_Thread_Handle                   EQU     00010108h
Unhook_V86_Int_Chain                    EQU     00010118h
Map_Flat                                EQU     0001001Ch
_HeapAllocate                           EQU     0001004Fh
_HeapReAllocate                         EQU     00010050h
_HeapFree                               EQU     00010051h
_HeapGetSize                            EQU     00010052h
_SelectorMapFlat                        EQU     0001006Eh
_Trace_Out_Service                      EQU     000100F3h
Unhook_Device_Service                   EQU     0001011Ch
_RegOpenKey      			EQU	00010148h
_RegCloseKey      			EQU	00010149h
_RegCreateKey     			EQU	0001014Ah
_RegDeleteKey    	                EQU     0001014Bh
_RegEnumKey      	                EQU     0001014Ch
_RegQueryValue   	                EQU     0001014Dh
_RegSetValue     	                EQU     0001014Eh
_RegDeleteValue  	                EQU     0001014Fh
_RegEnumValue    	                EQU     00010150h
_RegQueryValueEx 	                EQU     00010151h
_RegSetValueEx   	                EQU     00010152h
_RegFlushKey				EQU	00010155h
_lstrlen                                EQU     0001017Ch
_lstrcmpi                               EQU     0001018Eh
VWIN32_GetCurrentProcessHandle          EQU     002A000Dh
VWIN32_SysErrorBox                      EQU     002A001Ah
IFSMgr_Ring0_FileIO                     EQU     00400032h
UniToBCSPath                            EQU     00400041h
IFSMgr_InstallFileSystemApiHook         EQU     00400067h
IFSMgr_RemoveFileSystemApiHook          EQU     00400068h
;
; VXDLDR Services
;
_PELDR_LoadModule                       EQU     00270007h
_PELDR_LoadModuleEx                     EQU     00270011h
;
;
; Макросы для вызова VxD/VMM сервисов -----------------------------------------
;
?marg   macro   name, num
    ?AM&num &macro
        push	name
        &endm
    endm

?invprg macro   name1, name2
    name1&name2
    purge   name1&name2
    endm


VMMCall                 MACRO   Service, arglst

			??_argc = 0

			irp x,<arglst>
			        ??_argc = ??_argc + 1
				?marg   <x>,%??_argc
			endm

			?count = ??_argc

			rept    ??_argc
				?invprg <?AM>,%?count
				?count = ?count - 1
			endm

                        int     20h
                        DD      Service
			if	??_argc ne 0
			add	esp, 4*??_argc
			endif
			ENDM

VMMjmp			MACRO   Service, arglist

			??_argc = 0

			irp x,<arglst>
			        ??_argc = ??_argc + 1
				?marg   <x>,%??_argc
			endm

			?count = ??_argc

			rept    ??_argc
				?invprg <?AM>,%?count
				?count = ?count - 1
			endm

                        int     20h
                        DD      (Service OR 00008000h)  ; Set 'jmp' bit

			if	??_argc ne 0
			add	esp, 4*??_argc
			endif

			ENDM

VxDCall                 MACRO   Service, arglist

			??_argc = 0

			irp x,<arglst>
			        ??_argc = ??_argc + 1
				?marg   <x>,%??_argc
			endm

			?count = ??_argc

			rept    ??_argc
				?invprg <?AM>,%?count
				?count = ?count - 1
			endm

                        int     20h
                        DD      Service
			if	??_argc ne 0
			add	esp, 4*??_argc
			endif
			ENDM

VxDjmp                  MACRO   Service, arglist

			??_argc = 0

			irp x,<arglst>
			        ??_argc = ??_argc + 1
				?marg   <x>,%??_argc
			endm

			?count = ??_argc

			rept    ??_argc
				?invprg <?AM>,%?count
				?count = ?count - 1
			endm

                        int     20h
                        DD      (Service OR 00008000h)  ; Set 'jmp' bit
			if	??_argc ne 0
			add	esp, 4*??_argc
			endif
			ENDM

;
; Макросы ---------------------------------------------------------------------
;
Control_Dispatch MACRO Service, Procedure
			cmp	eax, Service
			je	Procedure
			ENDM
;

offset32 EQU <offset>

BeginProc macro Name
public Name
Name proc near
endm

EndProc macro Name
Name endp
endm

.LIST
