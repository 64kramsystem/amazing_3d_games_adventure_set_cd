#
# Borland C++ IDE generated makefile
#
.AUTODEPEND


#
# Borland C++ tools
#
IMPLIB  = Implib
BCCD32     = Bcc32 +BccD32.cfg
TLINK32 = TLink32
TLIB    = TLib
TASM32  = Tasm32
#
# IDE macros
#


#
# Options
#
IDE_LFLAGSD32 =  -LC:\BC4\LIB
LLATD32_ackdlib =  -Tpe -ax -LC:\BC4\LIB;C:\BC4\ACK
RLATD32_ackdlib = 
BLATD32_ackdlib = 
CNIEAT_ackdlib = -IC:\BC4\INCLUDE;C:\BC4\ACK -D
LNIEAT_ackdlib = -s
LEAT_ackdlib = $(LLATD32_ackdlib)
REAT_ackdlib = $(RLATD32_ackdlib)
BEAT_ackdlib = $(BLATD32_ackdlib)

#
# Dependency List
#
Dep_ack = \
   ack.lib

ack : BccD32.cfg $(Dep_ack)
  echo MakeNode 

Dep_ackdlib = \
   ackpcx.obj\
   ackrtn.obj\
   ackrtn1.obj\
   ackrtn2.obj\
   ackrtn3.obj\
   ackrtn4.obj\
   ackrtn5.obj\
   ackbkgd.obj\
   ackdata.obj\
   ackdisp.obj\
   ackdoor.obj\
   ackgif.obj\
   ackiff.obj\
   ackinit.obj\
   ackldbmp.obj\
   ackover.obj\
   ackpov.obj\
   ackutil.obj\
   ackview.obj\
   ackwrap.obj

ack.lib : $(Dep_ackdlib)
  $(TLIB) $< $(IDE_BFLAGS) $(BEAT_ackdlib) @&&|
 -+ackpcx.obj &
-+ackrtn.obj &
-+ackrtn1.obj &
-+ackrtn2.obj &
-+ackrtn3.obj &
-+ackrtn4.obj &
-+ackrtn5.obj &
-+ackbkgd.obj &
-+ackdata.obj &
-+ackdisp.obj &
-+ackdoor.obj &
-+ackgif.obj &
-+ackiff.obj &
-+ackinit.obj &
-+ackldbmp.obj &
-+ackover.obj &
-+ackpov.obj &
-+ackutil.obj &
-+ackview.obj &
-+ackwrap.obj
|

ackpcx.obj :  ackpcx.c
  $(BCCD32) -P- -c @&&|
 $(CEAT_ackdlib) $(CNIEAT_ackdlib) -o$@ ackpcx.c
|

ackrtn.obj :  ackrtn.asm
  $(TASM) @&&|
 /ml C:\BC4\ACK\ackrtn.asm ,ackrtn.obj
|

ackrtn1.obj :  ackrtn1.asm
  $(TASM) @&&|
 /ml C:\BC4\ACK\ackrtn1.asm ,ackrtn1.obj
|

ackrtn2.obj :  ackrtn2.asm
  $(TASM) @&&|
 /ml C:\BC4\ACK\ackrtn2.asm ,ackrtn2.obj
|

ackrtn3.obj :  ackrtn3.asm
  $(TASM) @&&|
 /ml C:\BC4\ACK\ackrtn3.asm ,ackrtn3.obj
|

ackrtn4.obj :  ackrtn4.asm
  $(TASM) @&&|
 /ml C:\BC4\ACK\ackrtn4.asm ,ackrtn4.obj
|

ackrtn5.obj :  ackrtn5.asm
  $(TASM) @&&|
 /ml C:\BC4\ACK\ackrtn5.asm ,ackrtn5.obj
|

ackbkgd.obj :  ackbkgd.c
  $(BCCD32) -P- -c @&&|
 $(CEAT_ackdlib) $(CNIEAT_ackdlib) -o$@ ackbkgd.c
|

ackdata.obj :  ackdata.c
  $(BCCD32) -P- -c @&&|
 $(CEAT_ackdlib) $(CNIEAT_ackdlib) -o$@ ackdata.c
|

ackdisp.obj :  ackdisp.c
  $(BCCD32) -P- -c @&&|
 $(CEAT_ackdlib) $(CNIEAT_ackdlib) -o$@ ackdisp.c
|

ackdoor.obj :  ackdoor.c
  $(BCCD32) -P- -c @&&|
 $(CEAT_ackdlib) $(CNIEAT_ackdlib) -o$@ ackdoor.c
|

ackgif.obj :  ackgif.c
  $(BCCD32) -P- -c @&&|
 $(CEAT_ackdlib) $(CNIEAT_ackdlib) -o$@ ackgif.c
|

ackiff.obj :  ackiff.c
  $(BCCD32) -P- -c @&&|
 $(CEAT_ackdlib) $(CNIEAT_ackdlib) -o$@ ackiff.c
|

ackinit.obj :  ackinit.c
  $(BCCD32) -P- -c @&&|
 $(CEAT_ackdlib) $(CNIEAT_ackdlib) -o$@ ackinit.c
|

ackldbmp.obj :  ackldbmp.c
  $(BCCD32) -P- -c @&&|
 $(CEAT_ackdlib) $(CNIEAT_ackdlib) -o$@ ackldbmp.c
|

ackover.obj :  ackover.c
  $(BCCD32) -P- -c @&&|
 $(CEAT_ackdlib) $(CNIEAT_ackdlib) -o$@ ackover.c
|

ackpov.obj :  ackpov.c
  $(BCCD32) -P- -c @&&|
 $(CEAT_ackdlib) $(CNIEAT_ackdlib) -o$@ ackpov.c
|

ackutil.obj :  ackutil.c
  $(BCCD32) -P- -c @&&|
 $(CEAT_ackdlib) $(CNIEAT_ackdlib) -o$@ ackutil.c
|

ackview.obj :  ackview.c
  $(BCCD32) -P- -c @&&|
 $(CEAT_ackdlib) $(CNIEAT_ackdlib) -o$@ ackview.c
|

ackwrap.obj :  ackwrap.c
  $(BCCD32) -P- -c @&&|
 $(CEAT_ackdlib) $(CNIEAT_ackdlib) -o$@ ackwrap.c
|

# Compiler configuration file
BccD32.cfg : 
   Copy &&|
-R
-v
-vi
-H
-H=ack.csm
-WX
-K-
-y
-R-
-H-
-H=
-Ot
-w-aus
-w-par
| $@


