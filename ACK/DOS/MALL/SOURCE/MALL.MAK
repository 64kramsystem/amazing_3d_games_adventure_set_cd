; Make file for Mall Demo
T=mouse.obj
C=mouse.c
D=wcl386 /c /j /oxt /l=dos4g mouse.c
T=mall.exe
C=mall.c
C=ack3d.h
C=ackeng.h
D=wcl386 /c /j /oxt /l=dos4g mall
T=mall.exe
C=mall.obj
C=ack3dl.lib
D=wcl386 /j /oxt /l=dos4g /fm=mall.map mall.obj mouse.obj modplay.obj ack3dl.lib

