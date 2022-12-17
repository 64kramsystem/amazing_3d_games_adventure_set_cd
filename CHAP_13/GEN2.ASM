;---------------------------------------------------------------------------
;
;	File:			GEN2.ASM
;	Programmer: 	Bob Provencher
;	Create Date:	11/9/94
;	Last Mod Date:	1/8/95
;	Description:	General function assembler module
;
;	Copyright (c) 1994, 1995. Aesir Software, Inc.
;	All Rights Reserved.
;
;---------------------------------------------------------------------------

.MODEL LARGE, PASCAL
include cmacro32.inc

ifndef SEGNAME
	SEGNAME equ <GEN2_TEXT32>
endif
 
createSeg %SEGNAME, CodeSeg, dword, use32, CODE

sBegin Data
sEnd Data

sBegin CodeSeg
		assumes cs, CodeSeg
		assumes ds, nothing
		assumes es, nothing

cProc	_hmemcpy,<FAR,PASCAL,PUBLIC>,<>
		parmD	dest
        parmD   src
        parmD   length
cBegin

    	push ds                         ; save regs
	    push es
	    push esi
    	push edi	

    	xor esi, esi
    	lds	si, src

    	xor	edi, edi
	    les	di, dest

        mov ecx, length
        mov edx, ecx
        shr ecx, 2
        cld
        rep movsd

        mov ecx, edx
        and ecx, 3
        rep movsb

    	pop	edi
    	pop	esi
    	pop	es
    	pop	ds

        mov eax, dest

        cEnd

cProc	_hmemset,<FAR,PASCAL,PUBLIC>,<>
		parmD	dest
		parmW   byte
		parmD   length

cBegin

		push es                         ; save regs
		push edi

		mov ax, byte
        mov	ah, al
		mov dx, ax
        shl eax, 16
        mov ax, dx

    	xor	edi, edi
	    les	di, dest

        mov ecx, length
        mov edx, ecx
        shr ecx, 2
        cld
        rep stosd

        mov ecx, edx
        and ecx, 3

        rep stosb

        mov eax, dest

    	pop	edi
    	pop	es

        cEnd

sEnd CodeSeg

end


