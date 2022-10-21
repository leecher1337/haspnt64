;
; HASP DOS driver for dongle call 2 emulation 
;
; (c) leecher@dose.0wnz.at   02/2022
;
; For this device driver to work i.e. in DosBox-X, you have to set
; DOS version number to 5.50 so that HASP thinks that it is running on
; Windows NTVDM
;

include "haspdrv.inc"

;****************************************************************
;*      WORK SPACE FOR OUR DEVICE DRIVER                        *
;****************************************************************

; The initialization parameters for CALL 2, will get filled from cmdline
; arguments (or you can patch them here and ignore errors on startup)
param_1     dd  0AAAAh
param_2     dd  0BBBBh
param_3     dd  0CCCCh
param_4     dd  0DDDDh

;ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß
; Open (init) device
;
OPEN    proc    near 
        xor     ax, ax
OPEN    endp

;ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß
; Close (uninit) device
;
CLOSE    proc    near 
        xor     ax, ax
CLOSE    endp

;ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß
; Input (read) routine 
;
INPUT    proc    near 
.386
        push    ds
        push    es
        push    di
        mov     cx,es:[di].rh4_count    ;load input count 
        les     di,es:[di+0Eh]          ;es:[di].rh4_buf_ofs
        
        call    Decrypt28
        
        cmp     es:[di].service, 1      ;we only emulate calls 1 and 2
        jz      is_hasp
        jb      unsupported
        cmp     es:[di].service, 2      ;we only emulate calls 1 and 2
        jnz     unsupported
        add     di, 8                   ;go to beginning of data
        mov     cx, 4                   ;copy param_1..4 to return buffer 
        lea     si, param_1
        repe    movsd
        jmp     return_ok
is_hasp:
        add     di, 8                   ;go to beginning of data
        mov     ax, 1
        stosd                           ; HASP found
        stosd                           ; On port 1
        xor     ax, ax
        stosd                           ; Success
        add     di, 4                   ;skip last param
return_ok:
        push    es
        pop     ds                      ;copy within same buffer 
        mov     cx, 4                   ;copy param_1..4 to return area 
        mov     si, di
        sub     si, 16                  ;4 params * 4 bytes = 16
        repe    movsd
        sub     di, 40                  ;scroll back to beginning of struct
        call    Encrypt28
        xor     ax, ax
        clc
        jmp     leave_input
unsupported:
        mov     ax, 8003h
        stc
leave_input:
        pop     di
        pop     es
        pop     ds
        ret
INPUT    endp

;****************************************************************
;*      HELPER PROCEDURES                                       *
;****************************************************************

;
; Encrypts out buffer
; Input: EDI - buffer for encrypt params
;
ENCRYPT28    proc    near 
LOCAL   Final_counter:word,counter:word,Cur_Word:word,tmp_decrypt:word,var_1:word,var_2:word,var_3:word
.386
        pushad
        xor     esi, esi
        and     edi, 0ffffh
        mov     word ptr counter, 0
        mov     word ptr Final_counter, 28h
        mov     word ptr tmp_decrypt, 11h
@encr_init:
        mov     si, counter
        mov     ax, es:[esi+edi]
        mov     Cur_Word, ax
        mov     var_1, 0
        mov     var_2, 0
@encr_loop:
        mov     bx, tmp_decrypt
        mov     cx, Cur_Word
        xor     bx, cx
        and     ebx, 1
        mov     var_3,  bx
        mov     esi, ebx
        mov     cx, word ptr var_1
        shl     si, cl
        mov     ax, var_2
        or      ax, si
        mov     var_2, ax
        mov     eax, 'IQ'
        mul     var_3
        mov     cx, ax
        mov     ax, tmp_decrypt
        sar     ax, 1
        mov     ebx, eax
        xor     bx, cx
        mov     tmp_decrypt, bx
        mov     ax, Cur_Word
        sar     ax, 1
        mov     Cur_Word, ax
        inc     var_1
        mov     eax, 10h
        cmp     ax, word ptr var_1
        jnz     @encr_loop
        mov     ax, var_2
        mov     si, counter
        mov     es:[esi+edi], ax
        add     counter, 2
        mov     ax, Final_counter
        cmp     ax, counter
        jnz     @encr_init
        popad
        ret
ENCRYPT28    endp

;
; Decrypts in buffer
; Input: EDI - buffer for decrypt params
;
DECRYPT28    proc    near 
.386
LOCAL   Final_counter:word,counter:word,Cur_Word:word,tmp_decrypt:word,var_1:word,var_2:word,var_3:word
        pushad
        xor     esi, esi
        and     edi, 0ffffh
        mov     counter, 0
        mov     Final_counter, 28h
        mov     word ptr tmp_decrypt, 11h
@decr_init:
        mov     si, counter
        mov     ax, es:[esi+edi]
        mov     Cur_Word, ax
        mov     var_1,  0
        mov     var_2,  0

@decr_loop:
        mov     bx, tmp_decrypt
        mov     cx, Cur_Word
        xor     bx, cx
        and     ebx, 1
        mov     si, bx
        mov     cx, var_1
        shl     si, cl
        mov     ax, var_2
        or      ax, si
        mov     var_2,  ax
        mov     ax, Cur_Word
        and     eax, 1
        mov     ebx, 'IQ'
        mul     bx
        mov     cx, ax
        mov     ax, tmp_decrypt
        sar     ax, 1
        mov     bx, ax
        xor     bx, cx
        mov     tmp_decrypt, bx
        mov     ax, Cur_Word
        sar     ax, 1
        mov     Cur_Word, ax
        inc     var_1
        mov     ax, 10h
        cmp     ax, var_1
        jnz     @decr_loop
        mov     ax, var_2
        mov     si, counter
        mov     es:[esi+edi], ax
        add     counter, 2
        mov     ax, Final_counter
        cmp     ax, counter
        jnz     @decr_init

        popad
        ret
DECRYPT28    endp

;****************************************************************
;*      END OF PROGRAM                                          *
;****************************************************************

;this procedure is called form the Initialization command and
;is executed only once. WE can tell DOS that the next available
;memory location (Break Adress) is here. This allows DOS to over
;write this code; we save space.

initial proc    near    ;display message on console
.386
        lea     dx, msg1        ;message to be displayed
        mov     ah, 9           ;display
        int     21h             ;DOS call
    
        push    ds
        les     di, dword ptr rh_ofs    ; DS:SI points to first char
        lds     si,es:[di.rh0_bpb_tb]   ; after DEVICE=
        xor     ebx, ebx                ; Init value register with 0
        mov     cx, 4                   ; We need 4 Parameters 
        pop     es                      ; es = old ds
        push    es
        lea     di, param_1             ; ES:DI points to param_1
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
        jne     not_blank
        cmp     ah, al                  ; If there was a prev. blank,
        je      found_param1            ; skip whitespace, otherwise it's next parameter 
        mov     es:[di], ebx
        add     di, 4
        xor     bx, bx                  ; Start with next value
        loop    found_param1            ; Parse next value, if left 
        jmp     initial_done            ; All 4 params parsed, done 
not_blank:
        mov     ah, al                  ; Store current char for comparison
        cmp     al, 13                  ; End of line?
        jne     not_cr
        loop    not_enough_param        ; If params left, bail out 
        mov     es:[di], ebx
        jmp     initial_done            ; All 4 params parsed, done 
not_cr:
        cmp     al, 'a'                 ; If hex, convert to uppercase
        jb      not_lower_hex
        cmp     al, 'f'
        ja      invalid_char
        sub     al, 32                  ; Convert to upper case hex 
not_lower_hex:
        cmp     al, 'F'
        ja      invalid_char
        cmp     al, '0'
        jb      invalid_char
        cmp     al, '9'
        jbe     is_number
        cmp     al, 'A'
        jb      invalid_char
        sub     al, 7
is_number:
        sub     al, 48
        shl     ebx, 4
        or      bl, al
        jmp     found_param1
invalid_char:
        lea     dx, msg3
        jmp     disp_str
not_enough_param:
        lea     dx, msg2
disp_str:
        pop     ds
        mov     ah, 9
        int     21h
        ret
initial_done:
        pop     ds
        ret
initial endp

msg1    db   'HASP emulation DOS driver, (c) leecher@dose.0wnz.at 2022',0dh,0ah,'$'
msg2    db   'We need 4 parameters in hex to emulate call reply',0dh,0ah,'$'
msg3    db   'Invalid parameter value, has to be hex',0dh,0ah,'$'


hasp    endp            ;end of hasp procedure
cseg    ends            ;end of cseg segment
        end     begin   ;end of program