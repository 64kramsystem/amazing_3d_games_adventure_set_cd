; Make file for medit program
T=mouse.obj
C=mouse.c
D=wcl386 /c /j /oxt /d2 /l=dos4g mouse
T=measm.obj
C=measm.asm
D=tasm /ml /zi measm,measm.obj,measm.lst;
T=m1util.obj
C=m1util.c
D=wcl386 /c /j /oxt /d2 /l=dos4g m1util
T=m1read.obj
C=m1read.c
D=wcl386 /c /j /oxt /d2 /l=dos4g m1read
T=m1.exe
C=m1.obj
C=m1.C
C=m1util.C
C=m1read.C
C=mouse.obj
C=measm.obj
D=wcl386 /j /oxt /d2 /l=dos4g /fm=m1.map m1.c m1util.obj m1read.obj mouse.obj measm.obj ack3dl.lib

