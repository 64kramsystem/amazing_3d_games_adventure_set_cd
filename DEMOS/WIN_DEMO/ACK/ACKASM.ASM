PAGE 80,132
TITLE - ACKASM.ASM - Assembly routines for 3D engine
.386P
	OPTION		OLDSTRUCTS
;==============================================================================
; COMMAND LINE ASSEMBLY
;	MASM /B63 /Z /D_ML FILENAME;
;				OR
;	ML /D_ML /H64 /c FILENAME.ASM
;
;  WHERE _ML SPECIFIES LARGE MEMORY MODEL
;==============================================================================
	INCLUDE		ET.MAC

	PUBLIC		?AckCeilFloor@@ZAXPEEHHHHHH@Z
	PUBLIC		?draw_col2@@ZAXHHHHHPEE00HH@Z
	PUBLIC		?draw_col3@@ZAXHHHPEE000HHHHH@Z
	PUBLIC		?AckCopyBackground@@ZAXPEE0HHHH@Z
	PUBLIC		?TBitBlts@@ZAXPIE0IHHHHH@Z
	PUBLIC		?draw_walls@@ZAXQEPEXQEPEE0PEE2HHHH@Z
	PUBLIC		?draw_object@@ZAHQEPEXPEEPEDIH11HHHHHHHHJJ@Z
	PUBLIC		?TintScreen@@ZAXPEEIHH@Z
	PUBLIC		?FloorSlice@@ZAXHHHPEUFLOORARRAY@@0JJJPEEHHHHPEH@Z
	PUBLIC		?long_divide@@ZAJJJ@Z

DGROUP	group	_CONST,_DATA,_BSS
	assume	cs: _TEXT,ds: DGROUP,ss: DGROUP
EXTRN   ?LastX1@@3HE:WORD
EXTRN   ?LastY1@@3HE:WORD
EXTRN   ?iLastX@@3HE:WORD
EXTRN   ?iLastY@@3HE:WORD
EXTRN   ?xMapPosn@@3HE:WORD
EXTRN   ?yMapPosn@@3HE:WORD
EXTRN   ?xTiles@@3HE:WORD
EXTRN   ?yTiles@@3HE:WORD

_CONST  segment word public use16 'CONST'
_CONST  ends

_DATA	segment word public use16 'DATA'
_DATA	ends

_BSS	segment word public use16 'BSS'
_BSS	ends

_TEXT	SEGMENT byte public use16 'CODE'
	ASSUME cs:_TEXT

;==============================================================================
;
;void AckCeilFloor@@ZAXPEEHHHHHH@Z(UCHAR far *,int,int,int,int,int,int);
;==============================================================================
CFBkgdBuffer	equ	[bp+ABASE]
CFLightFlag		equ	[bp+ABASE+4]
CFTopColor		equ	[bp+ABASE+6]
CFBottomColor	equ	[bp+ABASE+8]
CFWinStartY		equ	[bp+ABASE+10]
CFWinEndY			equ	[bp+ABASE+12]
CFCenterRow		equ	[bp+ABASE+14]

PBEGIN  ?AckCeilFloor@@ZAXPEEHHHHHH@Z
	push		bp
	mov		bp,sp
	push		si
	push		di

	les		di,dword ptr CFBkgdBuffer
	test	byte ptr CFLightFlag,1		;Is shading on
	jz		bcf_normal			;nope
	jmp		bcf_shade			;else create shaded buffer

bcf_normal:
	mov		cx,word ptr CFCenterRow
	sub		cx,word ptr CFWinStartY
	inc		cx
	imul	cx,320
	mov		ax,word ptr CFTopColor
	rep		stosb					;no need to be fast here
	mov		cx,word ptr CFWinEndY
	sub		cx,word ptr CFCenterRow
	imul	cx,320
	mov		ax,word ptr CFBottomColor
	rep		stosb
	jmp		bcf_done			;exit routine

bcf_shade:
	mov		bx,word ptr CFCenterRow
	sub		bx,word ptr CFWinStartY	  ;Number of video rows
	inc		bx
	mov		cx,bx				;Hold onto total rows
	shr		bx,3				;Turn into 8 zones
	mov		ax,word ptr CFTopColor
	mov		ah,al				;Top color to use
	mov		dx,0F0F0H	;4000H			;set shading for next zone
	call	FillInZone
	mov		dx,0F0C0H	;4040H
	call	FillInZone
	mov		dx,0C0C0H	;8040H
	call	FillInZone
	mov		dx,0C080H	;8080H
	call	FillInZone
	mov		dx,8080H	;0C080H
	call	FillInZone
	mov		dx,8040H	;0C0C0H
	call	FillInZone
	mov		dx,4040H	;0FFC0H
	call	FillInZone		;This takes care of top 8 zones
	mov		dx,4000H	;0FFFFH
	call	FillInZone
	xchg	ax,bx			;ax has rows per zone
	xchg	cx,bx			;bx has total rows for top color
	shl		ax,3			;x 8 to get total rows
	sub		bx,ax			;get remaining rows if odd
	jz		bcf010			;no odd rows to fill
	mov		dx,0FFFFH
	call	FillInZone

bcf010:
	mov		bx,word ptr CFWinEndY
	sub		bx,word ptr CFCenterRow
	mov		cx,bx
	shr		bx,3
	mov		ax,bx
	shl		ax,3
	sub		cx,ax		;any odd rows to fill?
	jz		bcf020		;nope
	push	bx		;save rows per zone
	mov		bx,cx
	mov		dx,0F0F0H	;0FFFFH
	call	FillInZone
	pop		bx

bcf020:
	mov		ax,word ptr CFBottomColor
	mov		ah,al
	mov		dx,04000H	;0FFFFH
	call	FillInZone
	mov		dx,04040H	;0FFC0H
	call	FillInZone
	mov		dx,08040H	;0C0C0H
	call	FillInZone
	mov		dx,08080H	;0C080H
	call	FillInZone
	mov		dx,0C0D0H	;8080H
	call	FillInZone
	mov		dx,0C0C0H	;8040H
	call	FillInZone
	mov		dx,0F0C0H	;4040H
	call	FillInZone
	mov		dx,0F0F0H	;4000H
	call	FillInZone


bcf_done:
	pop		di
	pop		si
	mov		sp,bp
	pop		bp
	ret
?AckCeilFloor@@ZAXPEEHHHHHH@Z endp

;==============================================================================
; Local routine, called by BuildCeilingFloor
; Entry: BX = number of rows to fill
;	 DX = Shading values for odd/even rows
;	 AX = base color value
;
; Exit: di advanced bx * 160 bytes, all other registers preserved
;
;==============================================================================
FillInZone proc near
	push	bx
	push	cx
	push	si
	mov		si,ax

fiz010:
	test	bx,1				;check odd/even row
	jz		fiz020
	or		ax,dx				; was or ax,dx
	jmp		short fiz030
fiz020:
	or		al,dh				;swap which bytes are used
	or		ah,dl
fiz030:
	mov		cx,160				;move in one row
	rep		stosw
	mov		ax,si				;restore base color
	dec		bx				;bump number of rows
	jnz		fiz010

	pop		si
	pop		cx
	pop		bx
	ret
FillInZone endp

;==============================================================================
; draw_col2(int Col, int slice, int dist, int width, int ht, UCHAR far *Wall,
;	    UCHAR far *Screen, UCHAR far *Pal, int light, int offset);
; Contributed by Jaimi McEntire
; Modified by John Lundy
;==============================================================================

DCcol		equ	[bp+ABASE]
DCslice equ	[bp+ABASE+2]
DCdist	equ	[bp+ABASE+4]
DCwidth equ	[bp+ABASE+6]
DCht		equ	[bp+ABASE+8]
DCwall	equ	[bp+ABASE+10]
DCscrn	equ	[bp+ABASE+14]
DCPal		equ	[bp+ABASE+18]
DClight equ	[bp+ABASE+22]
DChoff	equ	[bp+ABASE+24]

DCangle equ	[bp-2]

PBEGIN  ?draw_col2@@ZAXHHHHHPEE00HH@Z
	push	bp
	mov		bp,sp
	sub		sp,2
	push	ds
	push	si
	push	di

	xor		edx,edx
	mov		word ptr DCangle,dx

	cmp		word ptr DClight,dx
	lfs 	dx,dword ptr DCPal	; FS:DX at PalTables
	jz		zonelight

	;Good place for light modifier to modify zone
	mov		ax,DCdist			; get distance to object
	shl		ax,2				; multiply by 4 & divide by 256 to get zones
	cmp		ah,15				; make sure zone doesn't go beyond 15
	jl		zoneokay
	mov		ah,15
zoneokay:
	add 	dh,ah				; FS:DX points to corrected palette for zone

zonelight:
	lds		si,DCwall	 		; DS:SI point to wall buffer
	les 	di,dword ptr DCscrn ; ES:DI point to area we are painting

	add		di,DChoff			; Pick up offset to center of viewport
	add		di,word ptr DCcol	; es:di now point at starting area (top run)
	mov		bx,di				; dup center in bx
	add		bx,DCwidth			; es:bx points to 1 down (for bottom run)

	mov		ax,DCslice
	shl		ax,6				; mult slice by 64
	add		ax,si				; add in offset to wall bitmap
	mov		DCslice,ax			; save it
	xor		eax,eax				; clear eax for shading
	mov		cx,ax				; ht

looptop:	;	Do center of Screen UP
	movzx	ax,ch				; current ht
	mov		si,31				; base to start with
	sub		si,ax				; base - ht = row

	add		si,DCslice			; plus column start to point at the pixel.
	mov		al,ds:[si]			; mov al,offset slice[esi]
	or		al,al				; see if transparent
	jz		blank1				; don't draw this pixel
	mov		al,fs:[edx][eax]	; added this line to shade it.
	mov		es:[di],al

blank1:		;	Do center of Screen DOWN
	movzx	si,ch				; current ht
	add		si,32				; base + ht = row

	add		si,DCslice			; plus column start to point at the pixel.
	mov		al,ds:[si]			; mov al,offset slice[esi]
	or		al,al				; see if transparent
	jz		blank2				; don't draw this pixel
	mov		al,fs:[edx][eax]	; added this line to shade it.
	mov		es:[bx],al

blank2:
	mov		ax,DCwidth
	sub		di,ax				; point to the next two dest pixels.
	add		bx,ax				; point to the next two dest pixels.
	add		cx,word ptr DCdist	; add distance to ht adjuster.

;		Reached Top/Bottom of Bitmap
	cmp		ch,32				; cmp to 32 (1 half our bm size)
	jge		outtahere			; get out if bitmap done.

;		Reached Top/Bottom of Screen        
	mov		ax,DCangle
	cmp		ax,word ptr DCht	; if half height of screen,
	jge		outtahere			; then leave.
	inc		ax					; update screen row
	mov		DCangle,ax
	jmp		looptop				; continue

outtahere:
	pop		di
	pop		si
	pop		ds
	mov		sp,bp
	pop		bp
	ret
?draw_col2@@ZAXHHHHHPEE00HH@Z endp

;==============================================================================
; draw_col3(...)
; Created by John Lundy
;==============================================================================

DCcol		equ	[bp+ABASE]
DCslice equ	[bp+ABASE+2]
DCdist	equ	[bp+ABASE+4]
DCwall	equ	[bp+ABASE+6]
DCscrn	equ	[bp+ABASE+10]
DCpal		equ	[bp+ABASE+14]
DCtable	equ [bp+ABASE+18]
DCstart	equ	[bp+ABASE+22]
DClen		equ	[bp+ABASE+24]
DCwidth	equ [bp+ABASE+26]
DClight equ	[bp+ABASE+28]
DCadj   equ	[bp+ABASE+30]

PBEGIN  ?draw_col3@@ZAXHHHPEE000HHHHH@Z
	push	bp
	mov		bp,sp
	push	ds
	push	si
	push	di

	xor		edx,edx
	mov		esi,edx
	mov		ah,dh
	cmp		byte ptr DClight,dl
	lfs 	dx,dword ptr DCpal	; FS:DX at PalTables
	jz		zoner

	;Good place for light modifier to modify zone
	mov		ax,DCdist			; get distance to object
	shl		ax,2				; multiply by 2 & divide by 256 to get zone
zoner:
	add		ah,byte ptr DCadj	; additionally adjust light in this area
	cmp		ah,15				; make sure zone doesn't go beyond 15
	jl		zokay
	mov		ah,15
zokay:
	add 	dh,ah				; FS:DX points to corrected palette for zone

	lds		si,DCwall	 		; DS:SI point to wall buffer
	mov		ax,DCslice			; column start for this bitmap
	shl		ax,6				; mult slice by 64 (width of bitmaps)
	add		si,ax				; add in offset to wall bitmap

	les 	di,DCscrn			; ES:DI point to screen
	add		di,DCstart			; point at starting row
	add		di,word ptr DCcol	; point at starting col

	lgs		bx,DCtable			; magic table of offsets

	xor		eax,eax				; clear eax for shading
	mov		cx,DClen			; length of bits to move

loopback:	;	Do it
	mov		al,gs:[bx]			; get offset value
	mov		al,ds:[esi][eax]	; get correct bitmap bit
	or		al,al				; see if transparent
	jz		blank				; don't draw this pixel
	mov		al,fs:[edx][eax]	; added this line to shade it.
	mov		es:[di],al			; place in screen buffer

blank:
	inc		bx					; update pointer to magic bytes
	add		di,DCwidth			; next row down
	loop	loopback			; get out if bitmap done.

	pop		di
	pop		si
	pop		ds
	mov		sp,bp
	pop		bp
	ret
?draw_col3@@ZAXHHHPEE000HHHHH@Z endp

;==============================================================================
; AckCopyBackground(UCHAR far *scrn,UCHAR far *bkgd,int wt,int ht,int delta);
;==============================================================================
ACBscrn equ	[bp+ABASE]
ACBbkgd equ	[bp+ABASE+4]
ACBwt		equ	[bp+ABASE+8]
ACBht		equ	[bp+ABASE+10]
ACBlen	equ	[bp+ABASE+12]
ACBdlta	equ	[bp+ABASE+14]

PBEGIN  ?AckCopyBackground@@ZAXPEE0HHHH@Z
	push	bp
	mov		bp,sp
	push	ds
	push	si
	push	di

;------------------------------------------------------------------------------
; Quickly transfer the pre-built background ceiling and floor to the screen.
;------------------------------------------------------------------------------
	mov		ax,ACBdlta
	mov		bx,ACBwt
	mov		dx,ACBht

	lds		si,dword ptr ACBbkgd	      ;Pre-built buffer
	les		di,dword ptr ACBscrn	      ;Screen buffer

	or		ax,ax
	jnz		$100

	shr		bx,2		;DWORD count per row
	mov		ax,dx
	imul	bx
	mov		cx,ax		;DWORDS to move

	rep		movsd		;Move everything at once
	jmp		$200

$100:
	shl		ax,1
	shr		bx,1		;Moving words

$110:
	mov		cx,bx		;Length of data row
$120:
	movsw
	add		si,ax
	loop	$120		;Do all words

	add		si,ACBlen	;Add in length of row

	dec		dx			;Do all rows
	jnz		$110

$200:	
	pop		di
	pop		si
	pop		ds
	pop		bp
	ret
?AckCopyBackground@@ZAXPEE0HHHH@Z endp

;==============================================================================
; TBitBlts(UCHAR huge *pDestBits,UCHAR huge *pSrcBits,
;		UINT DRowOff,int DCol,int bmWidth,int bmHeight,intScrWidth);
; Created by John Lundy
;==============================================================================

DCscrn	equ	[bp+ABASE]
DCbmp		equ	[bp+ABASE+4]
DCoff		equ	[bp+ABASE+8]
DCcol		equ	[bp+ABASE+10]
DCwt		equ	[bp+ABASE+12]
DCht		equ	[bp+ABASE+14]
DCswt		equ	[bp+ABASE+16]
DCdlta	equ	[bp+ABASE+18]

PBEGIN  ?TBitBlts@@ZAXPIE0IHHHHH@Z
	push		bp
	mov		bp,sp
	push		ds
	push		si
	push		di
	
	les		di,DCscrn		; pointer to screen buffer
	add		di,DCoff		; point at starting row
	add		di,DCcol		; point at starting col
	
	lds		si,DCbmp		; pointer to bitmap
	
	mov		dx,DCht			; number of rows to copy
	mov		bx,DCwt			; width of bitamp

	mov		ax,DCdlta
	or		ax,ax
	jz		vloop

	shr		dx,1
	shr		bx,1

$10:
	mov		cx,bx
	xor		bx,bx

$20:
	lodsw						; get bitmap pixel
	or		al,al			; is bit transparent
	jz		$30			; don't draw this pixel
	mov		es:[di][bx],al	; put screen pixel

$30:
	inc		bx				; point to next bitmap pixel
	loop	$20			; loop through pixels

	add		si,DCwt
	sub		di,DCswt		; do next screen row
	dec		dx				; row counter
	jnz		$10			; loop through rows
	jmp		complete
	
vloop:
	mov		cx,bx			; number of pixels per row
	xor		bx,bx			; pixel number to move

hloop:
	lodsb						; get bitmap pixel
	or		al,al			; is bit transparent
	jz		skip			; don't draw this pixel
	mov		es:[di][bx],al	; put screen pixel

skip:
	inc		bx				; point to next bitmap pixel
	loop	hloop			; loop through pixels

	sub		di,DCswt		; do next screen row
	dec		dx				; row counter
	jnz		vloop			; loop through rows

complete:
	pop		di
	pop		si
	pop		ds
	mov		sp,bp
	pop		bp
	ret
?TBitBlts@@ZAXPIE0IHHHHH@Z endp

;==============================================================================
; draw_walls(...)
; Created by John Lundy
;==============================================================================

DCwalls		equ	[bp+ABASE]
DCbmaps		equ	[bp+ABASE+4]
DCtable		equ	[bp+ABASE+8]
DCscrn		equ	[bp+ABASE+12]
DCpal			equ	[bp+ABASE+16]
DCwidth		equ	[bp+ABASE+20]
DCheight	equ	[bp+ABASE+22]
DCupdown	equ	[bp+ABASE+24]
DClight 	equ	[bp+ABASE+26]

DClast	equ	[bp-2]
DCtmp		equ	[bp-4]
DCstart	equ	[bp-6]
DCbegin	equ	[bp-8]

wall			struc
wNumber			dw		?	;	/* Bitmap number of this wall	*/
wColumn			dw		?	;	/* Column of bitmap to display */
wDistance		dw		?	;	/* Distance to this column */
wLightAdj		dw		?	;	/* Light adjustment needed */
wall			ends

magic			struc
mBS_Pointer	dd		?	;	/* Pointer to bitmap offsets */
mBS_Start		dw		?	;	/* Starting offset on screen */
mBS_Length	dw		?	;	/* Length of data to move */
magic			ends

PBEGIN	?draw_walls@@ZAXQEPEXQEPEE0PEE2HHHH@Z
	push	bp
	mov		bp,sp
	sub		sp,8
	push	ds
	push	si
	push	di

	xor		eax,eax				; primarily to clear
	mov		edx,eax				;  the high word of
	mov		esi,eax				;   extended registers

	mov		ax,DCscrn			; figure out last col to do
	add		ax,DCwidth
	mov		DClast,ax

	lds		si,DCwalls			; pointer to WALLARRAY structure array
	mov		fs,DCpal+2			; FS:DX at PalTables

peat:
	mov		bx,ds:[si].wDistance	; get distance to object

	mov 	dx,DCpal				; FS:DX at PalTables, CX is temporary

	xor		ah,ah
	cmp		byte ptr DClight,ah
	jz		zone

	;	Good place for light modifier to modify zone
	mov		ax,bx				; use distance to object
	shl		ax,2				; multiply by 2 & divide by 256 to get zone
zone:
	add		ah,byte ptr ds:[si].wLightAdj	; additional light adjustment
	cmp		ah,15				; make sure zone doesn't go beyond 15
	jl		zok
	mov		ah,15
zok:
	add 	dh,ah				; FS:DX points to corrected palette for zone
	mov		DCtmp,dx		;	save Palette pointer

	mov		dx,ds:[si].wNumber	; number of bitmap to use
	shl		dx,2				; times 4 to get dword offset

	mov		ax,ds:[si].wColumn	; column start for this bitmap

	lds		si,DCbmaps	; pointer to wall bitmaps
	add		si,dx				; offset to bitmap pointer

	lds		si,ds:[si]	; DS:SI points to bitmap
	shl		ax,6				; mult slice by 64 (width of bitmaps)
	add		si,ax				; add to offset of wall bitmap

	xor		ax,ax				;	bitmap array offset
	mov		DCbegin,ax

	mov		ax,DCupdown	;	consider any up/down positioning
	cwd								;	into DX:AX
	div		bx					;	offset = (U/D << (8 - ViewLowRes)) / distance

	mov		dx,bx				; dup distance
	shl		dx,3				; 2**3=8 make pointer to magic structure
	lgs		bx,DCtable	; magic table of offsets
	add		bx,dx				; offset to table to use

	sub		ax,gs:[bx].mBS_Start	;	calc offset - centered offset
	mov		dx,DCheight	; starting row = (ViewHeight >> 1) - len - calc offset
	shr		dx,1
	sub		dx,ax
	mov		ax,gs:[bx].mBS_Length
	sub		dx,ax

	mov		DCstart,dx	;	calculate last position to paint
	add		dx,ax
	add		dx,ax

	cmp		dx,DCheight	;	are we out of bounds (bottom of screen)
	jl		$10
	mov		dx,DCheight	; ViewHeight - 1
	dec		dx
$10:
	mov		ax,DCstart
	or		ax,ax
	jge		$20
	sub		DCbegin,ax
	xor		ax,ax
$20:
	sub		dx,ax				;	calculate number of pixels to move
	mov		cx,dx

	mul		WORD PTR DCwidth

	lgs		bx,gs:[bx].mBS_Pointer			; GS:BX points to table
	add		bx,DCbegin

	mov		dx,DCtmp			;	reload Palette pointer

	les	 	di,DCscrn			; ES:DI point to screen
	add		di,ax

	xor		ah,ah

	push	bp					;	register operations in innermost loop
	mov		bp,DCwidth
back:	;	Do it
	mov		al,gs:[bx]			; get offset value
	mov		al,ds:[esi][eax]	; get correct bitmap bit
	or		al,al				; see if transparent
	jz		blk					; don't draw this pixel
	mov		al,fs:[edx][eax]	; added this line to shade it.
	mov		es:[di],al			; place in screen buffer

blk:
	inc		bx					; update pointer to magic bytes
	add		di,bp				; next row down
	loop	back				; get out if bitmap done.
	pop		bp

	mov		ax,DCscrn
	inc		ax
	cmp		ax,DClast			; add screen width
	jge		alldone	
	mov		DCscrn,ax			; keep next column value

	lds		si,DCwalls
	add		si,SIZEOF(wall)		; next WALLARRAY
	mov		DCwalls,si
	jmp		peat

alldone:
	pop		di
	pop		si
	pop		ds
	mov		sp,bp
	pop		bp
	ret
?draw_walls@@ZAXQEPEXQEPEE0PEE2HHHH@Z	endp

;==============================================================================
; draw_object(...)
; Created by John Lundy
;==============================================================================

DCwalls		equ	[bp+ABASE]
DCbmap		equ	[bp+ABASE+4]
DCtable		equ	[bp+ABASE+8]
DCbss			equ	[bp+ABASE+12]
DCbsl			equ	[bp+ABASE+14]
DCscrn		equ	[bp+ABASE+16]
DCpal			equ	[bp+ABASE+20]
DCwidth		equ	[bp+ABASE+24]
DCheight	equ	[bp+ABASE+26]
DCupdown	equ	[bp+ABASE+28]
DClight 	equ	[bp+ABASE+30]
DCadj			equ	[bp+ABASE+32]
DCdist		equ	[bp+ABASE+34]
DCfirst		equ	[bp+ABASE+36]
DClast		equ	[bp+ABASE+38]
DCwhole		equ	[bp+ABASE+40]
DCcol			equ	[bp+ABASE+42]
DCdelta		equ	[bp+ABASE+44]

DCcount	equ	[bp-2]
DCret		equ	[bp-4]
DCstart	equ	[bp-6]
DCbegin	equ	[bp-8]

PBEGIN	?draw_object@@ZAHQEPEXPEEPEDIH11HHHHHHHHJJ@Z
	push		bp
	mov		bp,sp
	sub		sp,8
	push		ds
	push		si
	push		di

	xor		esi,esi					; clear high word
	mov		DCret,si				; clear return value (FALSE)

	mov		ds,DCwalls+2		; DS:SI pointer to WALLARRAY structure element
	mov		es,DCscrn+2			; ES:DI point to screen
	xor		edx,edx					; clear high word
	mov		fs,DCpal+2			; FS:DX to PalTables
	mov		gs,DCtable+2		; GS:BX to magic table of offsets

	xor		ax,ax						;	bitmap array offset
	mov		DCbegin,ax

	mov		ax,DCupdown			;	consider any up/down positioning
	cwd										;	into DX:AX
	div		WORD PTR DCdist	;	offset = (U/D << (8 - ViewLowRes)) / distance
	sub		ax,DCbss				;	calc offset - centered offset

	mov		dx,DCheight			; starting row = (ViewHeight >> 1) - len - calc offset
	shr		dx,1
	sub		dx,ax
	mov		ax,DCbsl
	sub		dx,ax

	mov		DCstart,dx			;	calculate last position to paint
	add		dx,ax
	add		dx,ax

	cmp		dx,DCheight			;	are we out of bounds (bottom of screen)
	jl		$10
	mov		dx,DCheight			; ViewHeight - 1
	dec		dx
$10:
	mov		ax,DCstart
	or		ax,ax
	jge		$20
	sub		DCbegin,ax
	xor		ax,ax
$20:
	sub		dx,ax						;	calculate number of pixels to move
	mov		DCcount,dx

	mul		WORD PTR DCwidth
	mov		di,DCscrn				; screen buffer address
	add		di,ax						; row we start on
	add		DClast,di				; pointer to screen row/col end
	mov		si,DCfirst
	add		di,si						; first col to draw
	mov		DCscrn,di				; pointer to screen row/col start

	shl		si,3						; 2**3=8 is size of wall struct
	add		si,DCwalls			; index into pointer array
	mov		DCwalls,si
	
	mov		dx,DCpal				; FS:DX to PalTables
	xor		eax,eax					; clear high word
	cmp		byte ptr DClight,ah
	jz		zone1

	;	Good place for light modifier to modify zone
	mov		ax,DCdist				; use distance to object
	shl		ax,2						; multiply by 2 & divide by 256 to get zone
zone1:
	add		ah,DCadj				; additional light adjustment
	cmp		ah,15						; make sure zone doesn't go beyond 15
	jl		zok1
	mov		ah,15
zok1:
	add 	dh,ah						; FS:DX points to corrected palette for zone

peat1:
	mov		bx,DCdist				; get distance to object
	cmp		bx,ds:[si].wDistance	; is this col visible
	jg		nxtcol1

	mov		DCret,bx				; return distance (nonzero) if we draw anything

	lds		si,DCbmap	 			; pointer to bitmap
	mov		ax,DCcol
	shl		ax,6						; times 64 (width of bitmaps)
	add		si,ax						; offset to bitmap pointer

	mov		bx,DCtable			; GS:BX magic table of offsets
	add		bx,DCbegin

	mov		cx,DCcount

	xor		ah,ah

	push	bp							;	register operations in innermost loop
	mov		bp,DCwidth
back1:	;	Do it
	mov		al,gs:[bx]			; get offset value
	mov		al,ds:[esi][eax]	; get correct bitmap bit
	or		al,al						; see if transparent
	jz		blk1						; don't draw this pixel
	mov		al,fs:[edx][eax]	; added this line to shade it.
	mov		es:[di],al			; place in screen buffer


blk1:
	inc		bx							; update pointer to magic bytes
	add		di,bp						; next row down
	loop	back1						; get out if bitmap done.
	pop		bp

	lds		si,DCwalls
nxtcol1:
	add		si,SIZEOF(wall)	; next WALLARRAY
	mov		DCwalls,si

	mov		di,DCscrn
	inc		di
	cmp		di,DClast				; cmp to last col to draw
	jge		alldone1
	mov		DCscrn,di				; keep next column value

	mov		ebx,DCdelta			; calculate next column to draw
	add		DCwhole,ebx
	jmp		peat1

alldone1:
	pop		di
	pop		si
	pop		ds
	mov		ax,DCret
	mov		sp,bp
	pop		bp
	ret
?draw_object@@ZAHQEPEXPEEPEDIH11HHHHHHHHJJ@Z	endp

;==============================================================================
; TintScreen(...)
; Created by John Lundy
;==============================================================================

DCscrn	equ	[bp+ABASE]
DCsize	equ [bp+ABASE+4]
DCfreq	equ	[bp+ABASE+6]
DCpal		equ	[bp+ABASE+8]

PBEGIN	?TintScreen@@ZAXPEEIHH@Z
	push		bp
	mov		bp,sp
	push		ds
	push		si
	push		di

	les		di,DCscrn		; screen pointer
	xor		bx,bx				; offset into screen
	mov		al,DCpal		; palette entry to use
	mov		cx,DCfreq
	mov		dx,DCsize

again:
	mov		es:[di][bx],al	; new palette entry number
	add		bx,cx				; update offset
	cmp		bx,dx				; done with screen yet
	jb		again				; continue

	pop		di
	pop		si
	pop		ds
	mov		sp,bp
	pop		bp
	ret
?TintScreen@@ZAXPEEIHH@Z	endp

;==============================================================================
; FloorSlice(...)
; Created by John Lundy
;==============================================================================

FSx				equ	[bp+ABASE]
FSy				equ	[bp+ABASE+2]
FScol			equ	[bp+ABASE+4]
FSxFlrs		equ	[bp+ABASE+6]
FSyFlrs		equ	[bp+ABASE+10]
FScos			equ	[bp+ABASE+14]
FSsin			equ	[bp+ABASE+18]
FSview		equ	[bp+ABASE+22]
FSscrn		equ	[bp+ABASE+26]
FSwidth		equ	[bp+ABASE+30]
FSheight	equ	[bp+ABASE+32]
FSupdown	equ	[bp+ABASE+34]
FScenter	equ	[bp+ABASE+36]
FSbsf			equ	[bp+ABASE+38]

floor			struc
fNumber			dw		?	;		//	Bitmap number of floor tile
fxPos				dw		?	;		//	X Position
fyPos				dw		?	;		//	Y Position
fDistance		dw		?	;		//	Distance to this tile edge
fOffset			dw		?	;		//	Offset to tile relative to top of screen
floor			ends

FShigh	equ	[bp-2]
FSlow		equ	[bp-4]

PBEGIN	?FloorSlice@@ZAXHHHPEUFLOORARRAY@@0JJJPEEHHHHPEH@Z
	push		bp
	mov		bp,sp
	sub		sp,4
	push		ds
	push		si
	push		di

	xor		edi,edi
	les		di,FSscrn										;	es:di = ScreenBuffer

	xor		esi,esi
	lgs		si,FSbsf										;	gs:si = BS_Floors

	mov		cx,?xTiles@@3HE							;	Number of x tiles to process
	jcxz	$100
	lfs		bx,FSxFlrs									;	fs:bx floor array

$10:
	mov		ax,fs:[bx].fxPos
	sub		ax,FSx
	cwde
	imul	DWORD PTR FScos							;	(xPos - x) * InvCosTable[angle]
	mov		FSlow,eax
	movzx	eax,WORD PTR FShigh
	cmp		ax,800H											;	DistanceHigh >= MAX_DISTANCE
	jae		$100
	cmp		ax,10H											; DistanceHigh < 16 (min distance)
	jl		$20
	imul	DWORD PTR FSview						;	DistanceHigh * ViewCosTable
	mov		FSlow,eax
	movzx	eax,WORD PTR FShigh
	shl		ax,1
	push	gs:[esi][eax]								;	offset = BSFloor[DistanceHigh]

	mov		ax,FScenter									;	relative to standing position
	sub		ax,FSupdown									;	consider any up/down positioning
	cwd																;	into DX:AX
	idiv	WORD PTR FShigh							;	offset = (U/D << (8 - ViewLowRes)) / distance
	pop		dx
	add		ax,dx

	or		ax,ax
	jl		$20
	cmp		ax,FSheight
	jge		$20
	mul		WORD PTR FSwidth
	add		ax,FScol
	mov		BYTE PTR es:[edi][eax],244	;	ScreenBuffer[offset] = 244

$20:
	add		bx,SIZEOF(floor)						;	Next floor tile
	loop	$10

$100:
	mov		cx,?yTiles@@3HE							;	Number of y tiles to process
	jcxz	$200
	lfs		bx,FSyFlrs									;	fs:bx floor array

$110:
	mov		ax,fs:[bx].fyPos
	sub		ax,FSy
	cwde
	imul	DWORD PTR FSsin							;	(yPos - y) * InvSinTable
	mov		FSlow,eax
	movzx	eax,WORD PTR FShigh
	cmp		ax,800H											;	DistanceHigh >= MAX_DISTANCE
	jae		$200
	cmp		ax,10H											; DistanceHigh < 16 (min distance)
	jl		$120
	imul	DWORD PTR FSview						;	DistanceHigh * ViewCosTable
	mov		FSlow,eax
	movzx	eax,WORD PTR FShigh
	shl		ax,1
	push	gs:[esi][eax]								;	offset = BSFloor[DistanceHigh]

	mov		ax,FScenter									;	relative to standing position
	sub		ax,FSupdown									;	consider any up/down positioning
	cwd																;	into DX:AX
	idiv	WORD PTR FShigh							;	offset = (U/D << (8 - ViewLowRes)) / distance
	pop		dx
	add		ax,dx

	or		ax,ax
	jl		$120
	cmp		ax,FSheight
	jge		$120
	mul		WORD PTR FSwidth
	add		ax,FScol
	mov		BYTE PTR es:[edi][eax],244	;	ScreenBuffer[offset] = 244

$120:
	add		bx,SIZEOF(floor)						;	Next floor tile
	loop	$110

$200:
	pop		di
	pop		si
	pop		ds
	mov		sp,bp
	pop		bp
	ret
?FloorSlice@@ZAXHHHPEUFLOORARRAY@@0JJJPEEHHHHPEH@Z	endp

;==============================================================================
; long_divide(...)
; Created by John Lundy
;	This is here due to a bug in the Microsoft Large Memory Model Library when
;	a program is compiled for the 80386 processor. It appears that the library
;	does NOT return the long result in the DX:AX pair but instead returns the
;	result in the EAX. The expected return value should be in DX:AX as below.
;==============================================================================

LDden		equ	[bp+ABASE]
LDdiv		equ	[bp+ABASE+4]

PBEGIN	?long_divide@@ZAJJJ@Z

	push		bp
	mov		bp,sp

	mov		eax,LDdiv
	or		eax,eax
	jz		$10								;	don't allow divide by zero
	mov		ebx,eax
	mov		eax,LDden
	cdq
	idiv	DWORD PTR LDdiv
	pushd	eax								;	put result
	pop		ax								;	into AX
	pop		dx								;	and DX

$10:
	mov		sp,bp
	pop		bp
	ret

?long_divide@@ZAJJJ@Z	endp

_TEXT	ENDS
	END

?draw_scrn@@ZAXQEPEXQEPEEQEPEDPEIPEHPEE5HH@Z
?draw_scrn@@ZAXQEPEXQEPEE0PEE2HH@Z
