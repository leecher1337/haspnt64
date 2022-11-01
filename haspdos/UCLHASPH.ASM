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
; This is the header module with the input dispatch-routine 

public rh_ofs

include haspdrv.inc
include haspfunc.inc

EXTRN   INITIAL : PROC

GLOBAL  Buffer0x28  : PROC


;¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
; Open (init) device
;
OPEN    proc    near 
        xor     ax, ax
        retn
OPEN    endp

;¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
; Close (uninit) device
;
CLOSE   proc    near 
        xor     ax, ax
        retn
CLOSE   endp

;¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
; Input (read) routine 
;
INPUT   proc    near 
        mov     cx, 28h
        lds     si,es:[di+0Eh]          ; es:[di].rh4_buf_ofs
        mov     ax, di
        sub     sp, MEMO_BUFFER_SIZE
        mov     di, sp
        push    ax
        push    di                      ; di = Stack offset buffer
        push    es

        push    ss
        pop     es                      ; es = ss
        mov     bp, di
        cld
        repe    movsb                   ; ds:si -> es:di
        push    cs
        pop     ds                      ; ds = cs

        call    Buffer0x28
        pop     es                      ; Restore es
        push    ss
        pop     ds                      ; ds = ss

        pop     si
        pop     di
        mov     cx, 28h
        mov     es:[di].rh4_count, cx
        les     di,es:[di+0Eh]          ;es:[di].rh4_buf_ofs
        add     sp, MEMO_BUFFER_SIZE
        cld
        repe    movsb                   ; ds:si -> es:di

        xor     ax, ax
        clc
        retn
INPUT   endp


hasp    endp        ;end of hasp procedure
cseg    ends        ;end of cseg segment
    end begin       ;end of program
