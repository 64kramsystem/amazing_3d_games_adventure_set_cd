;---------------------------------------------------------------------------
;
;	File:			WINGBMP2.ASM
;	Programmer: 	Bob Provencher
;	Create Date:	12/22/94
;	Last Mod Date:	1/4/94
;	Description:	Tile assembler module
;
;	Copyright (c) 1994, 1995. Aesir Software, Inc.
;	All Rights Reserved.
;
;---------------------------------------------------------------------------

.MODEL LARGE, PASCAL
include cmacro32.inc

LINE_SHIFT	equ		5			; shift for one tile width or height
TILE_SHIFT	equ		10			; shift for one tile width and height
TILE_WIDTH	equ		32			; tile width
TILE_HEIGHT	equ		32			; tile height
MAP_WIDTH	equ		128			; map width
MAP_HEIGHT	equ		128			; map height
TILE_DWORDS equ     8           ; number of dwords in a TILE_WIDTH

ifndef SEGNAME
	SEGNAME equ <WINGBMP2_TEXT32>
endif

createSeg %SEGNAME, CodeSeg, dword, use32, CODE

sBegin Data
sEnd Data

sBegin CodeSeg
		assumes cs, CodeSeg
		assumes ds, nothing
		assumes es, nothing

cProc	Tile,<FAR,PASCAL,PUBLIC>,<>
		parmD	dib
		parmW	dib_w
		parmW	dib_h
		parmW	orient
		parmD	tiles
		parmD	map
		parmW	tile_w
		parmW	tile_h
cBegin

    	push ds                         ; save regs
	    push es
    	push fs
	    push esi
    	push edi	

    	xor esi, esi
    	lds	si, tiles

    	xor	edi, edi
	    les	di, dib

    	xor	ebx, ebx
    	lfs	bx, map

    	xor	eax, eax		            ; clear, prepare for width
	    mov	ax, dib_w		            ; get dib_width

		cmp	orient, 0		            ; if top down, begin
		jl	begin

bottom_up:

		push eax			            ; save width
		xor edx, edx
		mov	dx, dib_h		            ; get height
		dec	edx			                ; last line
		mul	edx			                ; offset to start in eax
		add	edi, eax		            ; start at bottom
		pop	eax			                ; get back width
		neg	eax			                ; neg width now in eax

begin:
		sub	eax, TILE_WIDTH				; increment minus a tile width
		xor	edx, edx		            ; clear high
		mov	dx, tile_h		            ; get row counter

		align	4

do_row:

		push ebx						; save map position
		push edx            			; save row counter
		push edi			            ; save dest for this row

		xor edx, edx
		mov	dx, tile_w					; tile counter

		align	4

do_tile:

		push edx        		    	; save tile counter
		push edi		        	    ; save dest of this tile

		xchg edx, eax					; save eax
		xor	eax, eax		            ; zero out eax
		mov	al, fs:[ ebx ]	        	; get tile number
		inc ebx							; next map pos
		shl	eax, TILE_SHIFT            	; offset to correct tile
		mov	esi, eax	            	; into source reg
		xchg eax, edx					; restore eax

		mov	edx, TILE_HEIGHT           	; line counter

		align	4

do_line:

		mov	ecx, TILE_DWORDS       	    ; width in dwords
		rep	movsd		            	; blit it
		add	edi, eax	            	; offset to next dest
		dec	edx			                ; any more lines?
		jnz	do_line		            	; do next line

		pop	edi		                	; get tile start back
		pop	edx			                ; restore tile counter

    	add	edi, TILE_WIDTH        		; point to next tile in row
    	dec	edx	                		; any more tiles?
    	jnz	do_tile	            		; do next tile

    	pop	edi		                 	; restore start of row
		pop	edx		                	; row counter
		pop ebx							; map pos

        add ebx, MAP_WIDTH				; next map row

		mov	ecx, eax	            	; one line signed width
        add	ecx, TILE_WIDTH				; add one tile width
		shl	ecx, LINE_SHIFT	           	; multiply by 32
		add	edi, ecx	            	; point to next dest row

    	dec	edx			                ; any more rows?
    	jnz	do_row			            ; no next row

    	pop	edi
    	pop	esi
    	pop	fs
    	pop	es
    	pop	ds

        cEnd

sEnd CodeSeg

end

