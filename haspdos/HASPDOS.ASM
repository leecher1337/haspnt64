;
; HASP DOS driver to interact with HASPVDD.DLL on NTVDM
;
; (c) leecher@dose.0wnz.at   03/2022
;
; You don't need this, use the original HASPDOS.SYS driver from
; Alladdin. But for reference purposes, you can see what the original
; driver does.
; 

include "haspdrv.inc"
include "isvbop.inc"

;****************************************************************
;*      WORK SPACE FOR OUR DEVICE DRIVER                        *
;****************************************************************
;**--------------- VDD information -------------**
VDD_DllName      db  "HASPVDD.DLL", 0
VDD_InitFunc     db  "VDDRegisterInit", 0
VDD_DispFunc     db  "VDDDispatch", 0
VDD_hVDD         dw  ? 


;ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß
; Open (init) device
;
OPEN    proc    near 
        push    es
        push    di
        push    ds
        pop     es
        mov     si, offset VDD_DllName  
        mov     di, offset VDD_InitFunc
        mov     bx, offset VDD_DispFunc 
        RegisterModule         ;calls the VDD 
        pop     di
        pop     es
        jnc     save_hVDD      ;if NC then success
        mov     ax, 8003h
        mov     si, 0
        and     ds:attribute[si], 8FFFh  ; Don't support IOCTL?
        retn

save_hVDD:
        mov  [VDD_hVDD],ax     ;save handle in ax
        xor     ax, ax
        clc
        retn
OPEN    endp

;ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß
; Close (uninit) device
;
CLOSE   proc    near 
        push    es
        push    di
        push    ds
        pop     es
        mov     ax, ds:[VDD_hVDD]
        UnRegisterModule
        pop     di
        pop     es
        jnc     close_ok      ;if NC then success
        mov     ax, 8003h
        mov     si, 0
        and     ds:attribute[si], 8FFFh  ; Don't support IOCTL?
        retn
close_ok:
        xor     ax, ax
        clc
        retn
CLOSE   endp

;ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß
; Input (read) routine 
;
INPUT   proc    near 
        push    es
        push    di
        mov     ax, cs:[VDD_hVDD]
        mov     dx, 1
        mov     cx, es:[di].rh4_count
        les     di, es:[di+0Eh]          ;es:[di].rh4_buf_of
        DispatchCall
        pop     di
        pop     es
        jnb     input_ok
        mov     ax, 8003h
        stc
        retn
input_ok:
        mov     es:[di].rh4_count, cx
        xor     ax, ax
        clc
        retn
INPUT   endp

;****************************************************************
;*      END OF PROGRAM                                          *
;****************************************************************

;this procedure is called form the Initialization command and
;is executed only once. WE can tell DOS that the next available
;memory location (Break Adress) is here. This allows DOS to over
;write this code; we save space.

initial	proc	near	;display message on console
        ; We have nothing to say 
        ret		    ;return to caller
initial	endp

hasp	endp		;end of hasp procedure
cseg	ends		;end of cseg segment
	end	begin	    ;end of program
