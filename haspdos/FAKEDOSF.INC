;
; HASP DOS driver, INT 21h hook, loader part
;
; (c) leecher@dose.0wnz.at   10/2022
;
; Fake DOS version number so that this driver gets used by the HASP
; code in the target application, if not running under NTVDM
; Also fixes driver name bug that causes hang (see description below)
;

; Check if we are running on real dos and have to fake Version number
; and fix bugs
public fakedos
fakedos	proc	near	;display message on console
        mov     ax, 3306h
        int     21h
        cmp     bx, 3205h               ;We are in NTVDM, nothing to do
        jz      done_fakedos
        push    es
        mov     ax, 3521h               ;get INT 21 interrupt vector
        int     21h
        mov     cs:old_int21h_ofs,bx
        mov     cs:old_int21h_seg,es
        mov     ah, 25h                 ; install new INT 21 handler
        mov     dx, offset int_21_handler
        int     21h
        pop     es
done_fakedos:
        ret
fakedos	endp
