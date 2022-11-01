;
; HASP DOS driver, main routine 
;
; (c) leecher@dose.0wnz.at   03/2022
;
; For this device driver to work i.e. in DosBox-X, you have to set
; DOS version number to 5.50 so that HASP thinks that it is running on
; Windows NTVDM
;

        title   HASP DOS  driver

;****************************************************************
;*      INSTRUCTING THE ASSEMBLER                               *
;****************************************************************

     cseg      segment   para public    'code'
     hasp      proc      far
               assume    cs:cseg, es:cseg, ds:cseg

include haspstr.inc

;****************************************************************
;*      MAIN PROCEDURE CODE                                     *
;****************************************************************

     begin:

;****************************************************************
;*      DEVICE HEADER REQUIRED BY DOS                           *
;****************************************************************

next_dev       dd   -1             ;no other drivers following
attribute      dw   0C840h         ;logical devices,open/close/RM
                                   ;IOCTL, character device
strategy       dw   dev_strategy   ;Strategy routine address
interrupt      dw   dev_interrupt  ;Interrupt routine address
dev_name       db   'HASPDOSDRV'   ;name of our driver

;****************************************************************
;*      WORK SPACE FOR OUR DEVICE DRIVER                        *
;****************************************************************

rh_ofs	dw   ?         ;offset  address of the request header
rh_seg	dw   ?         ;segment address of the request header
old_int2Fh_ofs  dw ?   ;offset  address of old INT 2F handler 
old_int2Fh_seg  dw ?   ;segment address of old INT 2F handler 

;CMDTAB is the command table that contains the word address
;for each command. The request header will contain the
;command desired. The INTERRUPT routine will jump through an
;address corresponding to the requested command to get to
;the appropriate command processing routine.

CMDTAB  label   byte            ;* = char devices only
        dw      INITIALIZATION  ; initialization
        dw      REPLY_0         ; media check (block only)
        dw      REPLY_0         ; build bpb
        dw      REPLY_0         ; ioctl in
        dw      INPUT           ; input (read)
        dw      REPLY_0         ;*non destructive input no wait
        dw      REPLY_0         ;*input status
        dw      REPLY_0         ;*input flush
        dw      REPLY_0         ; output (write)
        dw      REPLY_0         ; output (write) with verify
        dw      REPLY_0         ;*output status
        dw      REPLY_0         ;*output flush
        dw      REPLY_0         ; ioctl output
        dw      OPEN            ; device open
        dw      CLOSE           ; device close
        dw      REPLY_0         ; removeable media
        dw      REPLY_0         ; output til busy
        dw      REPLY_8003      ; command 17
        dw      REPLY_8003      ; command 18
        dw      REPLY_0         ; generic IOCTL
        dw      REPLY_8003      ; command 20
        dw      REPLY_8003      ; command 21
        dw      REPLY_8003      ; command 22
        dw      REPLY_0         ; get device
        dw      REPLY_0         ; set device 

;****************************************************************
;*      THE STRATEGY PROCEDURE                                  *
;****************************************************************

dev_strategy:  mov  cs:rh_ofs,bx   ;save the offset address
               mov  cs:rh_seg,es   ;save the segment address
               ret                 ;return to DOS


;****************************************************************
;*      THE INTERRUPT PROCEDURE                                 *
;****************************************************************

;device interrupt handler - 2nd call from DOS

dev_interrupt:

        push    ax              ;save machine state on entry
        push    bx
        push    cx
        push    dx
        push    ds
        push    es        
        push    di
        push    si
        push    bp
        push    cs
        pop     ds

        les     di, dword ptr rh_ofs

;jump to appropriate routine to process command

        mov     bl,es:[di].rh_cmd       ;get request header header command
        xor     bh, bh
        cmp     bx, 18h                 ;index above table limits?
        jle     make_call
        jmp     unk_cmd
make_call:
        shl     bx,1            ;times 2 for index into word table
        call    word ptr cmdtab[bx]
        les     di, dword ptr rh_ofs

done:	or	    ax ,0100h	    ;set done
        mov	    es:[di].rh_status, ax

        pop	    bp			    ;restore all registers
        pop	    si
        pop	    di
        pop	    es
        pop	    ds
        pop	    dx
        pop	    cx
        pop	    bx
        pop	    ax
        ret				        ;return to DOS
        
unk_cmd:
        mov     ax, 8003h	    ;set error bit and error code
        jmp	    done			;set done and exit




;****************************************************************
;*      YOUR LOCAL PROCEDURES                                   *
;****************************************************************

;ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß
; Return success
;
REPLY_0    proc    near 
        xor     ax, ax              ;return 0
        ret                         ;return to caller
REPLY_0    endp                 ;end of tone

;ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß
; Return not implemented
;
REPLY_8003    proc    near 
        mov  ax, 8003h
        ret                         ;return to caller
REPLY_8003    endp                   ;end of tone

;****************************************************************
;*      DOS COMMAND PROCESSING                                  *
;****************************************************************

;
; The Interrupt 2F handler for call 5000h
;
int_2f_handler:
        cmp     ax, 5000h
        jz      handle_hasp
        jmp     dword ptr cs:old_int2fh_ofs
handle_hasp:
        mov     ax, 192h
        mov     bx, 4553h
        mov     cx, 4143h
        mov     dx, 4950h
        iret

;ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß
; Init routine
;
INITIALIZATION    proc    near 
        push    es
        lea     ax,initial              ;set Break Addr. at initial
        mov     es:[di].rh0_brk_ofs,ax  ;store offset address
        mov     es:[di].rh0_brk_seg,cs  ;store segment address
        mov     ax, 352Fh               ;get INT 2f interrupt vector
        int     21h
        mov     cs:old_int2Fh_ofs,bx
        mov     cs:old_int2Fh_seg,es
        mov     ah, 25h                 ; install new INT 2f handler
        mov     dx, offset int_2f_handler
        int     21h
        call    initial                 ;display message
        pop     es
        ret
INITIALIZATION    endp
