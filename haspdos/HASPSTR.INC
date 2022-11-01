;
; HASP DOS driver, main routine 
;
; (c) leecher@dose.0wnz.at   03/2022
;
; For this device driver to work i.e. in DosBox-X, you have to set
; DOS version number to 5.50 so that HASP thinks that it is running on
; Windows NTVDM
;

;****************************************************************
;*      STRUCTURES                                              *
;****************************************************************


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
