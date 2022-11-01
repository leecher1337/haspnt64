;
; HASP DOS driver to do UCLHASP emulation
;
; (c) leecher@dose.0wnz.at   10/2022
;
; This provides a DOS level interface to UCLHASP and emulates a 
; simple dongle provided that you give it an universal Dongle Dump
; file.
;
; For this device driver to work i.e. in DosBox-X, you have to set
; DOS version number to 5.50 so that HASP thinks that it is running on
; Windows NTVDM
;
; This is the footer module that contains initialization code and
; data
;
.model tiny, StdCall
cseg    SEGMENT PUBLIC 'CODE'
        ASSUME  CS:cseg,DS:cseg

include haspfunc.inc
include haspstr.inc

GLOBAL  _t_Supported    : DWORD
GLOBAL  _heapSupported  : DWORD
GLOBAL  _heapMemory     : DWORD
GLOBAL  rh_ofs : DWORD

;****************************************************************
;*      WORK SPACE FOR OUR DEVICE DRIVER                        *
;****************************************************************
memoryTable  db 200h dup(0)
keyTable     Cell ?


;****************************************************************
;*      END OF PROGRAM                                          *
;****************************************************************

include fakedosh.inc

;this procedure is called form the Initialization command and
;is executed only once. WE can tell DOS that the next available
;memory location (Break Adress) is here. This allows DOS to over
;write this code; we save space.

public initial
initial proc    near    ;display message on console
.386
        lea     dx, msg1        ;message to be displayed
        mov     ah, 9           ;display
        int     21h             ;DOS call
        
        push    ds
        les     di, dword ptr rh_ofs    ; DS:SI points to first char
        lds     si,es:[di.rh0_bpb_tb]   ; after DEVICE=
        xor     bx, bx                  ; Init value register with 0
        pop     es                      ; es = old ds
        push    es
next_char_p1:
        lodsb 
        mov     ah, al
        cmp     al, ' '
        je      found_param1
        cmp     al, 13
        jne     next_char_p1
        jmp     not_enough_param
found_param1:
        lodsb
        cmp     al, ' '
        jz      found_param1
        mov     dx, si
        dec     dx
seek_cr:
        lodsb
        cmp     al, 13
        jne     seek_cr
        mov     byte ptr [si-1], 0       ; Zero-terminate string
        mov     ax, 3D00h
        int     21h             ; Open file
        pop     ds
        jc      fileError0
        mov     bx,ax           ; store "file handle" into bx
        mov     ax,4202h        ; ah = 42h, al = 2 (END + cx:dx offset)
        xor     cx,cx           ; cx = 0
        xor     dx,dx           ; dx = 0 (cx:dx = +0 offset)
        int     21h             ; will set the file pointer to end of file, returns dx:ax
        jc      fileError0      ; something went wrong, just exit
        cmp     ax, 732
        je      size_ok
        cmp     ax, 719
        je      size_ok
        cmp     ax, 716
        je      size_ok
        cmp     ax, 204
        je      size_ok
        cmp     ax, 220
        je      size_ok
        cmp     ax, 332
        je      size_ok
        cmp     ax, 693
        je      size_ok
        cmp     ax, 236
        je      size_ok
        cmp     ax, 588
        je      size_ok
        cmp     ax, 108
        je      size_ok
        cmp     ax, 604
        je      size_ok
        lea     dx, msg3        ; Invalid size of dump file
        jmp     disp_str
fileError0:
        jmp     fileError
size_ok:
        mov     si,ax
        mov     ax,4200h        ; ah = 42h, al = 0 (START))
        xor     cx,cx           ; cx = 0
        xor     dx,dx           ; dx = 0 (cx:dx = +0 offset)
        int     21h             ; Reset file pointer to start
        jc      fileError0
        lea     dx, keyTable
        mov     [_heapSupported], edx
        mov     cx, 2           ; Read 2 bytes
        mov     ah, 3Fh
        int     21h             ; Read
        jc      fileError0
        add     dx, 4
        mov     ah, 3Fh
        int     21h             ; Read PW2
        jc      fileError0
        add     dx, 6
        cmp     si, 693
        je      read_693
        cmp     si, 719
        je      read_693
        mov     cx, 4           ; Read Type, 4 bytes
        mov     ah, 3Fh
        int     21h             ; Read!
        xor     cx,cx
        mov     dx, 12h         ; Offset after keyType
        mov     ax,4201h        ; ah = 42h, al = 1 (CURRENT))
        int     21h             ; Seek!
        jc      fileError0
        lea     dx, memoryTable
        mov     cx, 4           ; Read SN
        mov     ah, 3Fh
        int     21h             ; Read!
        mov     ax,4201h        ; ah = 42h, al = 1 (CURRENT))
        xor     cx,cx
        cmp     si, 204
        je      read_204
        cmp     si, 332
        je      read_332
        cmp     si, 732
        je      read_732
        cmp     si, 716
        je      read_716
        jmp     read_ok          ; Dongle with no memory
               
read_332:
        mov     dx, 0BEh         ; Offset BE
        jmp     seek_read_204
        
read_204:
        mov     dx, 3Eh         ; Offset BE
seek_read_204:
        int     21h             ; Seek!
        jc      fileError       
        lea     dx, [memoryTable+4]
        mov     cx, 70h         ; Read Data
        mov     ah, 3Fh
        int     21h             ; Read!
        jc      fileError
        jmp     read_ok       
        
read_693:
        push    dx              ; Save destination for Type
        mov     ax,4201h        ; ah = 42h, al = 1 (CURRENT))
        xor     cx,cx
        mov     dx, 1           ; Offset 1, 1 byte = Type 
        int     21h             ; Seek!
        pop     dx              ; Restore Destination pointer for type
        jc      fileError
        mov     cx, 1           ; Read Type
        mov     ah, 3Fh
        int     21h             ; Read!
        mov     ax,4201h        ; ah = 42h, al = 1 (CURRENT))
        mov     dx, 0A9h        ; Offset A9 after PWD
        xor     cx, cx
        int     21h             ; Seek!
        jc      fileError
        lea     dx, memoryTable
        cmp     si,719
        je      read_719
        mov     cx, 1f4h        ; Read SN + data
        mov     ah, 3Fh
        int     21h             ; Read!
        jc      fileError
        jmp     read_ok
               
read_716:
        mov     dx, 0CEh        ; Offset BE
        int     21h             ; Seek!
        jmp     read_732_data
        
read_732:
        mov     dx, 0CEh        ; Offset CE
        int     21h             ; Seek!
read_732_data:
        jc      fileError       
        lea     dx, [memoryTable+4]
        jmp     read_719_data
        
fileError:
        lea     dx, msg4
        jmp     disp_str
        
read_719:
        mov     cx, 4           ; Read SN
        mov     ah, 3Fh
        int     21h             ; Read!
        jc      fileError
        add     dx, 4
        push    dx
        mov     ax,4201h        ; ah = 42h, al = 1 (CURRENT))
        xor     cx,cx
        mov     dx, 10h         ; Offset 10h
        int     21h             ; Seek!
        pop     dx              ; Restore Destination pointer for type
        jc      fileError
read_719_data:
        mov     cx, 1F0h        ; Read Data
        mov     ah, 3Fh
        int     21h             ; Read!
        jc      fileError
        jmp     read_ok

not_enough_param:
        pop     ds
        lea     dx, msg2       
        jmp     disp_str
        
read_ok:
        mov     ah, 3Eh
        int     21h             ; Close file!
        mov     [_t_Supported], 1
        lea     dx, memoryTable
        mov     [_heapMemory], edx
        
        call    fakedos
        lea     dx, msg5
        mov     ah, 9
        int     21h
        xor     ax, ax
        ret        

disp_str:
        mov     ah, 9
        int     21h
        mov     ax, 8003h       ; Error

        ret                     ;return to caller
initial endp

include fakedosf.inc

msg1    db   'UCLHASP emulation DOS driver V1.00',0dh,0ah,'$'
msg2    db   'We need universal dump file as parameter.',0dh,0ah,'$'
msg3    db   'Error: Invalid size of supplied dump file.',0dh,0ah,'$'
msg4    db   'Error reading the dump file.',0dh,0ah,'$'
msg5    db   'UCLHASP loaded.',0dh,0ah,'$'

;hasp   endp        ;end of hasp procedure
cseg    ends        ;end of cseg segment
;   end begin       ;end of program
end