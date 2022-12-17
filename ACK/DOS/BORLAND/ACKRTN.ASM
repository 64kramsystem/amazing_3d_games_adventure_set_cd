
	locals
	IDEAL
	JUMPS
	P386
	P387		; Allow 386 processor


	MASM
	.MODEL FLAT,STDCALL	;32-bit OS/2 model

.data

COLOR	dw	?
RETVAL	dw	?
MAPPOSN dw	?
XBEG	dd	?
XD	dd	?
YD	dd	?
YTEMP	dd	?
SY	dd	?
SX	dd	?



	.CODE
	IDEAL

	include "ackrtn.inc"

SC_INDEX	EQU	03C4h	; Sequencer Controller access
HIGH_ADDR	equ	80h	; High byte of screen offset
GC_INDEX	equ	3ceh	;Graphics Controller Index register
CRTC_INDEX	equ	3d4h	;CRT Controller Index register
MAP_MASK	equ	2	;Map Mask register index in SC
MEMORY_MODE	equ	4	;Memory Mode register index in SC
MAX_SCAN_LINE	equ	9	;Maximum Scan Line reg index in CRTC
START_ADDRESS_HIGH equ	0ch	;Start Address High reg index in CRTC
UNDERLINE	equ	14h	;Underline Location reg index in CRTC
MODE_CONTROL	equ	17h	;Mode Control register index in CRTC
READ_MAP	equ	4	;Read Map register index in GC
GRAPHICS_MODE	equ	5	;Graphics Mode register index in GC
MISCELLANEOUS	equ	6	;Miscellaneous register index in GC
SCREEN_WIDTH	equ	320	;# of pixels across screen
SCREEN_HEIGHT	equ	400	;# of scan lines on screen


	extrn	_ViewAngle:word
	extrn	_ScreenOffset:word
	extrn	_xPglobal:dword
	extrn	_yPglobal:dword
	extrn	_xBegGlobal:dword
	extrn	_yBegGlobal:dword
	extrn	_aeGlobal:dword
	extrn	_xGridGlobal:dword
	extrn	_yGridGlobal:dword
	extrn	_xPglobalHI:dword
	extrn	_yPglobalHI:dword
	extrn	_rbaTable:dword
	extrn	_rsHandle:word
	extrn	_LastX1:dword
	extrn	_LastY1:dword
	extrn	_iLastX:dword
	extrn	_iLastY;dword
	extrn	_MaxDistance:word
	extrn	_ErrorCode:word
	extrn	_xMapPosn:dword
	extrn	_yMapPosn:dword
	extrn	_Grid:dword
	extrn	_ObjGrid:dword

	extrn	_xSecretmPos:word
	extrn	_xSecretmPos1:word
	extrn	_xSecretColumn:word

	extrn	_ySecretmPos:word
	extrn	_ySecretmPos1:word
	extrn	_ySecretColumn:word

	extrn	_TotalSecret:word
	extrn	_ViewColumn:word
	extrn	_SinTable:dword
	extrn	_CosTable:dword
	extrn	_LongTanTable:dword
	extrn	_LongInvTanTable:dword
	extrn	_InvCosTable:byte
	extrn	_InvSinTable:byte
	extrn	_LongCosTable:dword
	extrn	_ViewCosTable:dword
	extrn	_xNextTable:dword
	extrn	_yNextTable:dword

	extrn	_LastMapPosn:word
	extrn	_LastObjectHit:word
	extrn	_TotalObjects:word
	extrn	_FoundObjectCount:word
	extrn	_ObjectsSeen:byte
	extrn	_MoveObjectCount:word
	extrn	_MoveObjectList:byte
	extrn	_ObjNumber:byte
	extrn	_ObjRelDist:byte
	extrn	_ObjColumn:byte

	extrn	_x_xPos:dword
	extrn	_x_yPos:dword
	extrn	_x_xNext:dword
	extrn	_x_yNext:dword
	extrn	_y_xPos:dword
	extrn	_y_yPos:dword
	extrn	_y_xNext:dword
	extrn	_y_yNext:dword

	extrn	_Floorscr:dword
	extrn	_Floors1:dword
	extrn	_Floors2:dword
	extrn	_FloorscrTop:dword
	extrn	_Floorptr2:dword
	extrn	_Floorht:word
	extrn	_Floorwt:word

	extrn	_gScrnBuffer:dword
	extrn	_gWinStartY:word
	extrn	_gWinStartX:word
	extrn	_gWinWidth:word
	extrn	_gWinHeight:word

	extrn	_gWinFullWidth:word
	extrn	_gWinDWORDS:dword
	extrn	_gWinStartOffset:dword

	extrn	_VidSeg:dword
	extrn	_AckKeys:byte

	extrn	_OurDataSeg:word

    ACKPUBS AckInkey
    ACKPUBS AckKbdInt
    ACKPUBS AckPutVideo
    ACKPUBS AckGetVideo
    ACKPUBS AckCopyToVideo
    ACKPUBS AckSetPalette
    ACKPUBS AckDisplayScreen
    ACKPUBS AckInitVideoSelector
    ACKPUBS AckGetIntVector
    ACKPUBS AckSetIntVector
    ACKPUBS AckSetVGAmode
    ACKPUBS AckSetTextMode
    ACKPUBS AckDrawPage
    ACKPUBS xRaySetup
    ACKPUBS yRaySetup
    ACKPUBS xRayCast
    ACKPUBS yRayCast

;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
; Checks for a keystroke and returns 0 if none, else scan/char in AX
; This routine should NOT be used if the keyboard vector has been changed
; to the AckKbdInt routine below.
; unsigned short AckInkey(void);
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
ACKPROC AckInkey
    push    ebx
    mov	    ax,0100h
    int	    16h
    mov	    bx,0
    jz	    ackinkey_10
    xor	    ax,ax
    int	    16h
    mov	    bx,ax

ackinkey_10:
    mov	    ax,bx
    pop	    ebx
    ret
    endp

;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
; Interrupt 9 keyboard handler. Places keys into the keyboard array so they
; can be checked by the application.
; Do NOT call this routine directly!!!
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
ACKPROC AckKbdInt
    push    eax
    push    ebx
    push    esi
    push    ds
    xor	    ebx,ebx
    in	    al,60h
    mov	    bl,al
    in	    al,61h
    mov	    ah,al
    or	    al,80h
    out	    61h,al
    mov	    al,ah
    out	    61h,al
    mov	    al,20h
    out	    20h,al
    mov	    ax,cs:[word ptr _OurDataSeg]
    mov	    ds,ax
    mov	    esi,offset _AckKeys
    mov	    eax,ebx
    and	    eax,127
	mov	    [byte ptr esi+eax],1
	test	ebx,128
	jz	    aki_10
    mov	    [byte ptr esi+eax],0

aki_10:
    pop	    ds
    pop	    esi
    pop	    ebx
    pop	    eax
    iretd
    endp

;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
; Older version which now simply calls AckDrawPage.
; void AckDisplayScreen(void);
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
ACKPROC AckDisplayScreen
	push	ebp
	ACKCALL AckDrawPage
	pop	ebp
	xor	eax,eax
	ret
	endp

;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
; Puts a single byte onto the video
; void AckPutVideo(unsigned int offset,unsigned char color);
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
ACKPROC AckPutVideo
	push	ebp
	mov	ebp,esp
	push	es
	push	ebx
	push	edi
	mov	bx,[word ptr _VidSeg+2]
	mov	es,bx
	movzx	edi,[word ptr _VidSeg]
	add	edi,[dword ptr ebp+8]
	mov	edx,[ebp+12]
	mov	[edi],dl
	pop	edi
	pop	ebx
	pop	es
	pop	ebp
	ret
	endp

;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
; Retrieves a single byte from the video
; unsigned char AckGetVideo(unsigned int offset);
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
ACKPROC AckGetVideo
	push	ebp
	mov	ebp,esp
	push	ds
	push	ebx
	push	esi
	movzx	esi,[word ptr _VidSeg]
	add	esi,[dword ptr ebp+8]
	mov	ax,[word ptr _VidSeg+2]
	mov	ds,ax
	mov	al,[esi]
	mov	ah,0
	pop	esi
	pop	ebx
	pop	ds
	pop	ebp
	ret
	endp

;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
; Copies a block from the data segment buffer to the video screen
; void AckCopyToVideo(unsigned int SrcOff,unsigned int len);
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
ACKPROC AckCopyToVideo
	push	ebp
	mov	ebp,esp
	push	es
	push	ebx
	push	ecx
	push	edx
	push	esi
	push	edi
	mov	esi,[ebp+8]
	mov	ecx,[ebp+12]
	movzx	edi,[word ptr _VidSeg]
	mov	ax,[word ptr _VidSeg+2]
	mov	es,ax
	rep	movsb
	pop	edi
	pop	esi
	pop	edx
	pop	ecx
	pop	ebx
	pop	es
	pop	ebp
	ret
	endp


;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
; Retrieves a selector to Video memory and stores it in the VidSeg global
; void AckInitVideoSelector(void);
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
ACKPROC AckInitVideoSelector
    push    ebx
    push    ecx
    push    edx
    mov	    ax,2	; Allocate selector for real mode address
    mov	    bx,0A000h	; Get Video address
    int	    31h
    mov	    [word ptr _VidSeg+2],ax
    mov	    [word ptr _VidSeg],0
    pop	    edx
    pop	    ecx
    pop	    ebx
    ret
    endp

;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
; Retrieves a protected mode interrupt vector.
; void AckGetIntVector(int VectorNumber,int *sel,int *off);
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
ACKPROC AckGetIntVector
    push    ebp
    mov	    ebp,esp
    push    ebx
    push    ecx
    push    edx
    push    es
    mov	    ebx,[ebp+8]
    mov	    eax,204h
    int	    31h
    mov	    ebx,[ebp+12]
    mov	    [ebx],cx
    mov	    ebx,[ebp+16]
    mov	    [ebx],dx
    pop	    es
;   mov	    eax,ecx
    pop	    edx
    pop	    ecx
    pop	    ebx
    pop	    ebp
    ret
    endp

;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
; Sets a protected mode interrupt handler.
; void AckSetIntVector(int VecNum,unsigned int VecSel,unsigned int VecOff);
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
ACKPROC AckSetIntVector
    push    ebp
    mov	    ebp,esp
    push    ebx
    push    ecx
    push    edx
    mov	    ebx,[ebp+8]
    mov	    ecx,[ebp+12]
    mov	    edx,[ebp+16]
;;  mov	    cx,cs
    mov	    eax,205h
    cli
    int	    31h
    sti
    pop	    edx
    pop	    ecx
    pop	    ebx
    pop	    ebp
    ret
    endp

;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
;
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
ACKPROC AckSetPalette
    push    ebp
    mov	    ebp,esp
    push    ds
    push    ebx
    push    ecx
    push    edx
    mov	    esi,[ebp+8]
    mov	    ebx,0
    mov	    ecx,256
    mov	    dx,3c8h

asp_loop:
    mov	    al,bl
    out	    dx,al
    inc	    bx
    inc	    dx
    lodsb
    out	    dx,al
    lodsb
    out	    dx,al
    lodsb
    out	    dx,al
    dec	    dx
    loop    asp_loop

    pop	    edx
    pop	    ecx
    pop	    ebx
    pop	    ds
    pop	    ebp
    ret
    endp

;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
;
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
ACKPROC AckSetVGAmode
    push    ebx
    push    ecx
    push    edx
    mov	    ax,0013h  ;AH = 0 means mode set, AL = 13h selects
    int	    10h	      ;BIOS video interrupt
    pop	    edx
    pop	    ecx
    pop	    ebx
    ret

;------------------------------------------------------------------------------
; Uncomment the following code to setup a 320x200 modeX environment
;------------------------------------------------------------------------------
;; Change CPU addressing of video memory to linear (not odd/even,
;; chain, or chain 4), to allow us to access all 256K of display
;; memory. When this is done, VGA memory will look just like memory
;; in modes 10h and 12h, except that each byte of display memory will
;; control one 256-color pixel, with 4 adjacent pixels at any given
;; address, one pixel per plane.
;;------------------------------------------------------------------------------
;    mov     dx,SC_INDEX
;    mov     al,MEMORY_MODE
;    out     dx,al
;    inc     dx
;    in	     al,dx
;    and     al,not 08h	     ;turn off chain 4
;    or	     al,04h	     ;turn off odd/even
;    out     dx,al
;    mov     dx,GC_INDEX
;    mov     al,GRAPHICS_MODE
;    out     dx,al
;    inc     dx
;    in	     al,dx
;    and     al,not 10h	     ;turn off odd/even
;    out     dx,al
;    dec     dx
;    mov     al,MISCELLANEOUS
;    out     dx,al
;    inc     dx
;    in	     al,dx
;    and     al,not 02h	     ;turn off chain
;    out     dx,al
;    mov     dx,SC_INDEX
;    mov     ax,(0fh shl 8) + MAP_MASK
;    out     dx,ax
;
;    mov     dx,CRTC_INDEX
;;------------------------------------------------------------------------------
;; Change CRTC scanning from doubleword mode to byte mode, allowing
;; the CRTC to scan more than 64K of video data.
;;------------------------------------------------------------------------------
;    mov     al,UNDERLINE
;    out     dx,al
;    inc     dx
;    in	     al,dx
;    and     al,not 40h	     ;turn off doubleword
;    out     dx,al
;    dec     dx
;    mov     al,MODE_CONTROL
;    out     dx,al
;    inc     dx
;    in	     al,dx
;    or	     al,40h  ;turn on the byte mode bit, so memory is
;			 ; scanned for video data in a purely
;			 ; linear way, just as in modes 10h and 12h
;    out     dx,al
;    ret
    endp


;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
;
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
ACKPROC AckSetTextMode
    push    ebx
    push    ecx
    push    edx
    mov	    ax,3
    int	    10h
    pop	    edx
    pop	    ecx
    pop	    ebx
    ret
    endp

;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
;
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
ACKPROC AckDrawPage
    push    esi
    push    edi
    push    ebx
    push    ecx
    push    edx
    push    es
    push    ds

;;  mov	    edi,[_VidSeg]
    mov	    edi,0
    mov	    di,[word ptr _VidSeg]
    mov	    esi,[_gScrnBuffer]
    cmp	    [word ptr _gWinFullWidth],0
    jz	    short dp_smallscreen

    mov	    eax,[_gWinStartOffset]
    add	    edi,eax
    add	    esi,eax
    mov	    ecx,[_gWinDWORDS]

    mov	    ax,[word ptr _VidSeg+2]
    mov	    es,ax
;;  mov	    ds,ax

    mov	    dx,3dah

fp020:
    in	    al,dx	    ;Wait until vertical retrace is on
    test    al,8
    jz	    fp020

fp030:
    in	    al,dx	    ;Wait until vertical retrace is off
    test    al,8
    jnz	    fp030

    rep	    movsd

    pop	    ds
    pop	    es
    pop	    edx
    pop	    ecx
    pop	    ebx
    pop	    edi
    pop	    esi
    ret

dp_smallscreen:
    mov	    eax,[_gWinStartOffset]
    add	    edi,eax
    add	    esi,eax
    movzx   eax,[_gWinStartX]
    add	    edi,eax
    add	    esi,eax
    mov	    dx,[_gWinHeight]
    inc	    dx
    movzx   ebx,[_gWinWidth]
    mov	    ebp,320
    sub	    ebp,ebx	    ;width to advance pointers

    mov	    ax,[word ptr _VidSeg+2]
    mov	    es,ax
;;  mov	    ds,ax

dp010:
    mov	    ecx,ebx
    shr	    ecx,1
    rep	    movsw
    rcl	    ecx,1
    rep	    movsb
    add	    edi,ebp
    add	    esi,ebp
    dec	    dx
    jnz	    dp010

dp090:
    pop	    ds
    pop	    es
    pop	    edx
    pop	    ecx
    pop	    ebx
    pop	    edi
    pop	    esi
    ret
    endp


;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
;
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
ACKPROC xRaySetup
    push    esi
    push    ebx
    push    ecx
    push    edx
    mov	    dx,[_ViewAngle]
    movzx   esi,dx
    shl	    esi,2			    ; // Hold onto viewangle * 4
    mov	    ebx,[_yNextTable]
    mov	    eax,[esi+ebx]
    mov	    [dword ptr _x_yNext],eax
    cmp	    dx,INT_ANGLE_270
    jg	    short inbetween
    cmp	    dx,INT_ANGLE_90
    jge	    short not_inbetween

inbetween:

    mov	    ecx,[_xBegGlobal]
    add	    ecx,64
;-; movzx   ecx,cx
    mov	    [dword ptr _x_xPos],ecx
    mov	    [dword ptr _x_xNext],large 64
    jmp	    short xr_cont

not_inbetween:

    movzx   ecx,[word ptr _xBegGlobal]
    mov	    [_x_xPos],ecx
    mov	    [dword ptr _x_xNext],large -64
    neg	    [dword ptr _x_yNext]

xr_cont:

    movzx   eax,[word ptr _xPglobal]
    sub	    ecx,eax
    mov	    ebx,[dword ptr _LongTanTable]
    imul    ecx,[dword ptr esi+ebx]
    add	    ecx,[dword ptr _yPglobalHI]
    mov	    [dword ptr _x_yPos],ecx
    pop	    edx
    pop	    ecx
    pop	    ebx
    pop	    esi
    ret
    endp

;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
;
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
ACKPROC xRayCast
;;  push    ebp
;;  mov	    ebp,esp
;;  sub	    esp,50
    push    esi
    push    edi
    push    ebx
    push    ecx
    push    edx

    mov	    [word ptr RETVAL],0

looptop:

    mov	    edx,[dword ptr _x_xPos]
    cmp	    edx,large 0
    jl	    short getout
    cmp	    edx,large 4096
    jge	    short getout
    mov	    eax,[_x_yPos]
    cmp	    eax,large 0
    jl	    short getout
    cmp	    eax,large 010000000h
    jl	    short inbounds

getout:
    jmp	    loopdone

inbounds:
    sar	    eax,16
    and	    eax,-64
    sar	    edx,6
    add	    eax,edx
    cmp	    eax,4095
    ja	    getout
    mov	    esi,eax

    shl	    eax,1
    mov	    ebx,[dword ptr _ObjGrid]
    mov	    ax,[word ptr ebx+eax]
    or	    ax,ax
    jz	    short no_obj

    and	    eax,0FFh
    mov	    [word ptr COLOR],ax

    movzx   ecx,[_FoundObjectCount]	;// Get number of current objects seen
    mov	    ebx,ecx
    test    ecx,ecx
    jz	    short nofound
    mov	    edi,offset _ObjectsSeen
    repne   scasb			;// See if this object already seen
    jz	    short no_obj		;// Yes, ignore this ray then

nofound:
    mov	    edi,offset _ObjectsSeen
    mov	    [edi+ebx],al
    inc	    ebx
    mov	    [word ptr _FoundObjectCount],bx

no_obj:
    mov	    eax,esi
    mov	    ebx,[dword ptr _xGridGlobal]
    mov	    cx,[ebx+eax*2]
    test    cx,cx
    jnz	    short wall_here
    jmp	    next_square

wall_here:
    mov	    [word ptr COLOR],cx
    mov	    [_xMapPosn],esi

    mov	    ebx,[dword ptr _x_xPos]
    mov	    [dword ptr _iLastX],ebx
    mov	    edx,[dword ptr _x_yPos]
    mov	    [dword ptr _LastY1],edx

    test    cx,DOOR_WALL
    jz	    short not_door

    mov	    ebx,edx		    ;// ebx = edx = yPos
    sar	    edx,16
    and	    edx,large 00000FFC0h
    mov	    [dword ptr YD],edx	    ;  // yd = (yPos >> FP_SHIFT) & 0xFFC0;
    add	    edx,large 64
    mov	    [dword ptr XD],edx	      ;// xd = yd + GRID_SIZE;

    mov	    eax,[dword ptr _x_yNext]
    sar	    eax,1
    add	    ebx,eax
    mov	    [dword ptr YTEMP],ebx   ; // yTemp = yPos + (yNext >> 1);
    sar	    ebx,16

    cmp	    ebx,[dword ptr YD]
    jl	    short door_not_visible
    cmp	    ebx,[dword ptr XD]
    jle	    short door_visible

door_not_visible:
    mov	    eax,[dword ptr _x_xNext]
    add	    [dword ptr _x_xPos],eax	   ;// xPos += xNext;
    mov	    eax,[dword ptr _x_yNext]
    add	    [dword ptr _x_yPos],eax	   ;// yPos += yNext;
    jmp	    looptop

door_visible:
    mov	    eax,[dword ptr YTEMP]
    mov	    [dword ptr _LastY1],eax	;// LastY1 = yTemp;
    mov	    eax,[dword ptr _x_xNext]
    sar	    eax,1
    add	    [dword ptr _iLastX],eax	;// iLastX += xNext >> 1;

not_door:
    test    cx,DOOR_TYPE_SECRET
    jz	    short br_no_secret
    cmp	    [word ptr _xSecretColumn],0
    jne	    short secret_door

br_no_secret:
    jmp	    short give_color

secret_door:
    movzx   eax,[word ptr _xSecretColumn]
    movzx   ebx,[word ptr _ViewAngle]
    shl	    ebx,2

    add	    ebx,[dword ptr _LongTanTable]
    imul    eax,[dword ptr ebx]
    mov	    [dword ptr SY],eax
    mov	    ebx,[dword ptr _x_yPos]
    add	    eax,ebx
    mov	    [dword ptr YTEMP],eax
    sar	    eax,16
    sar	    ebx,16
    and	    ebx,large 00000FFC0h
    mov	    [dword ptr YD],ebx
    mov	    ecx,ebx
    add	    ecx,large 64

    cmp	    eax,ebx
    jl	    short next_square
    cmp	    eax,ecx
    jle	    short secret_is_visible

secret_not_visible:
    jmp	    short next_square

secret_is_visible:
    mov	    eax,[dword ptr YTEMP]
    mov	    [dword ptr _LastY1],eax
    mov	    eax,[dword ptr _xSecretColumn]
    add	    [dword ptr _iLastX],eax

give_color:
    movzx   eax,[word ptr COLOR]
    jmp	    short xRayDone

next_square:
    mov	    eax,[dword ptr _x_xNext]
    add	    [dword ptr _x_xPos],eax
    mov	    eax,[dword ptr _x_yNext]
    add	    [dword ptr _x_yPos],eax
    jmp	    looptop

loopdone:
    movzx   eax,[word ptr RETVAL]

xRayDone:
    pop	    edx
    pop	    ecx
    pop	    ebx
    pop	    edi
    pop	    esi
;;  mov	    esp,ebp
;;  pop	    ebp
    ret
    endp


;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
;
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
ACKPROC yRaySetup
    push    esi
    push    ebx
    push    ecx
    push    edx
    mov	    dx,[word _ViewAngle]
    movzx   esi,dx
    shl	    esi,2		    ;	 // Hold onto viewangle * 4
    mov	    ebx,[dword ptr _xNextTable]
    mov	    eax,[esi+ebx]
    mov	    [dword ptr _y_xNext],eax
    cmp	    dx,INT_ANGLE_180
    jge	    short y_not_inbetween

y_inbetween:
    mov	    ecx,[_yBegGlobal]
    add	    ecx,64
    mov	    [dword ptr _y_yPos],ecx
    mov	    [dword ptr _y_yNext],large 64
    jmp	    short y_yr_cont

y_not_inbetween:
    mov	    ecx,[_yBegGlobal]
    mov	    [_y_yPos],ecx
    mov	    [dword ptr _y_yNext],large -64
    neg	    [dword ptr _y_xNext]

y_yr_cont:
    mov	    eax,[_yPglobal]
    sub	    ecx,eax
    mov	    ebx,[dword ptr _LongInvTanTable]
    imul    ecx,[dword ptr esi+ebx]
    add	    ecx,[dword ptr _xPglobalHI]
    mov	    [dword ptr _y_xPos],ecx
    pop	    edx
    pop	    ecx
    pop	    ebx
    pop	    esi
    ret
    endp

;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
;
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
ACKPROC	yRayCast
;;  push    ebp
;;  mov	    ebp,esp
;;  sub	    esp,50
    push    esi
    push    edi
    push    ebx
    push    ecx
    push    edx
    mov	    [word ptr RETVAL],0

y_looptop:
    mov	    edx,[dword ptr _y_xPos]
    cmp	    edx,large 0
    jl	    short y_getout
    cmp	    edx,large 010000000h
    jge	    short y_getout
    mov	    eax,[_y_yPos]
    cmp	    eax,large 0
    jl	    short y_getout
    cmp	    eax,large 4096
    jl	    short y_inbounds

y_getout:
    jmp	    loopdone

y_inbounds:
    sar	    edx,22
    and	    eax,-64
    add	    eax,edx
    cmp	    eax,4095
    ja	    y_getout

    mov	    esi,eax

    shl	    eax,1
    mov	    ebx,[dword ptr _ObjGrid]
    mov	    ax,[ebx+eax]
    or	    ax,ax
    jz	    short y_no_obj

    and	    eax,0FFh
    mov	    [word ptr COLOR],ax

    movzx   ecx,[_FoundObjectCount]
    mov	    ebx,ecx
    jcxz    short y_nofound
    mov	    edi,offset _ObjectsSeen
    repne   scasb
    jz	    short y_no_obj

y_nofound:
    mov	    edi,offset _ObjectsSeen
    mov	    [edi+ebx],al
    inc	    bx
    mov	    [word ptr _FoundObjectCount],bx

y_no_obj:
    mov	    eax,esi
    shl	    eax,1
    mov	    ebx,[dword ptr _yGridGlobal]
    mov	    cx,[ebx+eax]
    test    cx,cx
    jnz	    short y_wall_here
    jmp	    y_next_square

y_wall_here:
    mov	    [word ptr COLOR],cx
    mov	    [_yMapPosn],esi

    mov	    ebx,[dword ptr _y_xPos]
    mov	    [dword ptr _LastX1],ebx
    mov	    edx,[dword ptr _y_yPos]
    mov	    [dword ptr _iLastY],edx
    mov	    edx,ebx

    test    cx,DOOR_WALL
    jz	    short y_not_door

    sar	    ebx,16
    and	    ebx,large 00000FFC0h
    mov	    [dword ptr YD],ebx

    add	    ebx,large 64
    mov	    [dword ptr XD],ebx

    mov	    eax,[dword ptr _y_xNext]
    sar	    eax,1
    add	    edx,eax
    mov	    [dword ptr YTEMP],edx
    sar	    edx,16

    cmp	    edx,[dword ptr YD]
    jl	    short y_door_not_visible
    cmp	    edx,[dword ptr XD]
    jle	    short y_door_visible

y_door_not_visible:
    mov	    eax,[dword ptr _y_xNext]
    add	    [dword ptr _y_xPos],eax
    mov	    eax,[dword ptr _y_yNext]
    add	    [dword ptr _y_yPos],eax
    jmp	    y_looptop

y_door_visible:
    mov	    eax,[dword ptr YTEMP]
    mov	    [dword ptr _LastX1],eax
    mov	    eax,[dword ptr _y_yNext]
    sar	    eax,1
    add	    [dword ptr _iLastY],eax

y_not_door:
    test    cx,DOOR_TYPE_SECRET
    jz	    short y_br_no_secret
    cmp	    [word ptr _ySecretColumn],0
    jne	    short y_secret_door

y_br_no_secret:
    jmp	    y_give_color

y_secret_door:
    movzx   eax,[word ptr _ySecretColumn]
    movzx   edx,[word ptr _ViewAngle]
    mov	    ebx,[dword ptr _LongInvTanTable]
    imul    eax,[dword ptr ebx+edx*4]
    mov	    [dword ptr SX],eax
    mov	    ebx,[dword ptr _y_xPos]
    add	    eax,ebx
    mov	    [dword ptr YTEMP],eax
    sar	    eax,16
    sar	    ebx,16
    and	    ebx,large 00000FFC0h
    mov	    ecx,ebx
    add	    ecx,large 64

    cmp	    eax,ebx
    jl	    short y_secret_not_visible
    cmp	    eax,ecx
    jle	    short y_secret_is_visible

y_secret_not_visible:
    jmp	    y_door_not_visible

y_secret_is_visible:
    mov	    eax,[dword ptr YTEMP]
    mov	    [dword ptr _LastX1],eax
    mov	    eax,[dword ptr _ySecretColumn]
    add	    [dword ptr _iLastY],eax

y_give_color:
    movzx   eax,[word ptr COLOR]
    jmp	    short yRayDone

y_next_square:
    mov	    eax,[dword ptr _y_xNext]
    add	    [dword ptr _y_xPos],eax
    mov	    eax,[dword ptr _y_yNext]
    add	    [dword ptr _y_yPos],eax
    jmp	    y_looptop

y_loopdone:
    movzx   eax,[word ptr RETVAL]

yRayDone:
    pop	    edx
    pop	    ecx
    pop	    ebx
    pop	    edi
    pop	    esi
;;  mov	    esp,ebp
;;  pop	    ebp
    ret
    endp

	 end

