; Make file for new ACK engine library
; This make file is for use by MK.EXE from Ack Software
T=mouse.obj
C=mouse.c
D=wcl386 /c /j /oxt /l=dos4g mouse.c
T=fdemo.exe
C=fdemo.c
C=fdemo.obj
C=ack3dl.lib
D=wcl386 /j /oxt /l=dos4g /fm=fdemo.map fdemo.c mouse.obj modplay.obj ack3dl.lib

