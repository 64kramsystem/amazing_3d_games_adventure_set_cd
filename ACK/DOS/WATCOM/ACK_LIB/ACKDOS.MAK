; Make file for new ACK engine library
; This make file uses the MK.EXE utility program from Ack Software
T=lmobj\ackinit.obj
C=ackinit.c
C=ack3d.h
C=ackeng.h
D=wcl386 /c /j /oxt /l=dos4g /folmobj\ackinit.obj ackinit
T=lmobj\ackldbmp.obj
C=ackldbmp.c
C=ack3d.h
C=ackeng.h
D=wcl386 /c /j /oxt /l=dos4g /folmobj\ackldbmp.obj ackldbmp
T=lmobj\ackover.obj
C=ackover.c
C=ack3d.h
C=ackeng.h
D=wcl386 /c /j /oxt /l=dos4g /folmobj\ackover.obj ackover
T=lmobj\ackbkgd.obj
C=ackbkgd.c
C=ack3d.h
C=ackeng.h
D=wcl386 /c /j /oxt /l=dos4g /folmobj\ackbkgd.obj ackbkgd
T=lmobj\ackview.obj
C=ackview.c
C=ack3d.h
C=ackeng.h
D=wcl386 /c /j /oxt /l=dos4g /folmobj\ackview.obj ackview
T=lmobj\ackdisp.obj
C=ackdisp.c
C=ack3d.h
C=ackeng.h
D=wcl386 /c /j /oxt /l=dos4g /folmobj\ackdisp.obj ackdisp
T=lmobj\ackwrap.obj
C=ackwrap.c
C=ack3d.h
C=ackeng.h
D=wcl386 /c /j /oxt /l=dos4g /folmobj\ackwrap.obj ackwrap
T=lmobj\ackutil.obj
C=ackutil.c
C=ack3d.h
C=ackeng.h
D=wcl386 /c /j /oxt /l=dos4g /folmobj\ackutil.obj ackutil
T=lmobj\ackiff.obj
C=ackiff.c
C=ack3d.h
C=ackeng.h
D=wcl386 /c /j /oxt /l=dos4g /folmobj\ackiff.obj ackiff
T=lmobj\ackgif.obj
C=ackgif.c
C=ack3d.h
C=ackeng.h
D=wcl386 /c /j /oxt /l=dos4g /folmobj\ackgif.obj ackgif
T=lmobj\ackpov.obj
C=ackpov.c
C=ack3d.h
C=ackeng.h
D=wcl386 /c /j /oxt /l=dos4g /folmobj\ackpov.obj ackpov
T=lmobj\ackdata.obj
C=ackdata.c
C=ack3d.h
C=ackeng.h
D=wcl386 /c /j /oxt /l=dos4g /folmobj\ackdata.obj ackdata.c
T=lmobj\ackdoor.obj
C=ackdoor.c
C=ack3d.h
C=ackeng.h
D=wcl386 /c /j /oxt /l=dos4g /folmobj\ackdoor.obj ackdoor.c
T=mouse.obj
C=mouse.c
D=wcl386 /c /j /oxt /l=dos4g mouse.c
T=lmobj\ackrtn.obj
C=ackrtn.asm
D=tasm /ml ackrtn,lmobj\ackrtn.obj;
T=lmobj\ackrtn1.obj
C=ackrtn1.asm
D=tasm /ml ackrtn1,lmobj\ackrtn1.obj;
T=lmobj\ackrtn2.obj
C=ackrtn2.asm
D=tasm /ml ackrtn2,lmobj\ackrtn2.obj;
T=lmobj\ackrtn3.obj
C=ackrtn3.asm
D=tasm /ml ackrtn3,lmobj\ackrtn3.obj;
T=lmobj\ackrtn4.obj
C=ackrtn4.asm
D=tasm /ml ackrtn4,lmobj\ackrtn4.obj;
T=lmobj\ackrtn5.obj
C=ackrtn5.asm
D=tasm /ml ackrtn5,lmobj\ackrtn5.obj;
E=CD lmobj
E=del ack3dl.lib
E=wlib ack3dl.lib @alibl.rsp
E=CD..

