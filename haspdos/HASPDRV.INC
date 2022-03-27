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

;structures

haspbuf         struc   ;HASP input structure
ticks           dd      ?     ;?
service         dw      ?     ;service code 
service2        dw      ?
r_ax_ret        dd      ?
r_bx            dd      ?
r_cx            dd      ?
r_dx            dd      ?
r_di            dd      ?
r_si            dd      ?
r_es            dd      ?
r_ax            dd      ?
haspbuf         ends


rh              struc   ;fixed request header structure
rh_len          db      ?     ;00 len of packet
rh_unit         db      ?     ;01 unit code (block devices only)
rh_cmd          db      ?     ;02 device driver command
rh_status       dw      ?     ;03 returned by the device driver
rh_res1         dd      ?     ;05 reserved
rh_res2         dd      ?     ;09 reserved
rh              ends          ;

rh0             struc   ;request header for Initialization (command 0)
rh0_rh          db      size rh dup (?) ;fixed request header portion
rh0_nunits      db      ?     ;number of units (block devices only)
rh0_brk_ofs     dw      ?     ;offset address for break
rh0_brk_seg     dw      ?     ;segment address for break
rh0_bpb_tb      dd      ?     ;pointer to BPB array
rh0_drv_ltr     db      ?     ;first available drive (DOS 3+) (block only)
rh0             ends          ;

rh4             struc   ;request header for INPUT         (command 4)
rh4_rh          db      size rh dup(?)  ;fixed request header portion
rh4_media       db      ?     ;0D media descriptor from DPB
rh4_buf_ofs     dw      ?     ;0E offset address of data transfer area
rh4_buf_seg     dw      ?     ;10 segment address of data transfer area
rh4_count       dw      ?     ;12 transfer count (sectors for block)
                              ;  (bytes for character)
rh4_start       dw      ?     ;14 start sector number (block only)
rh4             ends          ;16

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
        xor     ax, ax                  ; everything ok
        ret
INITIALIZATION    endp
