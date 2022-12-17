; Make file for medit program
T=mouse.obj
C=mouse.c
D=wcl386 /c /j /oxt /d2 /l=dos4g mouse
T=miff.obj
C=miff.c
D=wcl386 /c /j /oxt /d2 /l=dos4g miff
T=mgif.obj
C=mgif.c
D=wcl386 /c /j /oxt /d2 /l=dos4g mgif
T=measm.obj
C=measm.asm
D=tasm /ml /zi measm,measm.obj,measm.lst;
T=medit.exe
C=medit.obj
C=medit.C
C=mouse.obj
C=measm.obj
C=miff.obj
C=mgif.obj
C=mapedit.h
D=wcl386 /j /oxt /d2 /l=dos4g /fm=medf.map medit.c mouse.obj miff.obj mgif.obj measm.obj

