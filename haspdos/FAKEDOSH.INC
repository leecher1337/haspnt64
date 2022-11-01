;
; HASP DOS driver, INT 21h hook, persistent part
;
; (c) leecher@dose.0wnz.at   10/2022
;
; Fake DOS version number so that this driver gets used by the HASP
; code in the target application, if not running under NTVDM
; Also fixes driver name bug that causes hang (see description below)
;

old_int21h_ofs  dw ?   ;offset  address of old INT 2F handler 
old_int21h_seg  dw ?   ;segment address of old INT 2F handler 

int_21_handler:
        cmp     ax, 3306h
        jz      handle_getver
        cmp     ah, 3Dh
        jz      handle_fileopen
handle_default:
        jmp     dword ptr cs:old_int21h_ofs
handle_getver:
        cmp bx, 100h        ; Seems to me in HASP code
        jnz handle_default  ; bx is always 100h, so additional chk
        mov bx, 3205h
        mov dx, 1000h
        iret
handle_fileopen:
; There was a misunderstanding about the length of driver names in the
; device header by the HASP authors, as it seems. For some unknown reason,
; they thought that it can be 10 chars, whereas only 8 characters for a
; device name are valid. This doesn't seem to be an issue in NTVDM DOS,
; but on plain DOS 6.22, DOS just locks up when using a device name >8
; chars on OPEN call (see MS-DOS Kernel dir2.asm routine ChkDev).
; Therefore we have to repair this mess here:
        push cx
        push es
        push si
        push di
        mov si, dx
        push cs
        pop es
        mov cx, 5
        mov di, 10
        repe cmpsw
        jne not_me
        mov byte ptr ds:[si-2], 0
not_me:
        pop di
        pop si
        pop es
        pop cx
        jmp     dword ptr cs:old_int21h_ofs
