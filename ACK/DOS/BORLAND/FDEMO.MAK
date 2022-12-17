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
LLATD32_fdemodexe =  -Tpe -ax -LC:\BC4\LIB;C:\BC4\LIB
RLATD32_fdemodexe = 
BLATD32_fdemodexe = 
CNIEAT_fdemodexe = -IC:\BC4\INCLUDE;C:\BC4\INCLUDE -D
LNIEAT_fdemodexe = -x
LEAT_fdemodexe = $(LLATD32_fdemodexe)
REAT_fdemodexe = $(RLATD32_fdemodexe)
BEAT_fdemodexe = $(BLATD32_fdemodexe)
CLATW16_ackdlib = 
LLATW16_ackdlib = 
RLATW16_ackdlib = 
BLATW16_ackdlib = 
CEAT_ackdlib = $(CEAT_fdemodexe) $(CLATW16_ackdlib)
CNIEAT_ackdlib = -IC:\BC4\INCLUDE;C:\BC4\INCLUDE -D
LNIEAT_ackdlib = -x
LEAT_ackdlib = $(LEAT_fdemodexe) $(LLATW16_ackdlib)
REAT_ackdlib = $(REAT_fdemodexe) $(RLATW16_ackdlib)
BEAT_ackdlib = $(BEAT_fdemodexe) $(BLATW16_ackdlib)

#
# Dependency List
#
Dep_fdemo = \
   fdemo.exe

fdemo : BccD32.cfg $(Dep_fdemo)
  echo MakeNode 

Dep_fdemodexe = \
   modplay.obj\
   ack.lib\
   mouse.obj\
   fdemo.obj

fdemo.exe : $(Dep_fdemodexe)
  $(TLINK32) @&&|
 /v $(IDE_LFLAGSD32) $(LEAT_fdemodexe) $(LNIEAT_fdemodexe) +
C:\BC4\LIB\c0x32.obj+
modplay.obj+
mouse.obj+
fdemo.obj
$<,$*
ack.lib+
C:\BC4\LIB\dpmi32.lib+
C:\BC4\LIB\bidsf.lib+
C:\BC4\LIB\tv32.lib+
C:\BC4\LIB\cw32.lib

|

mouse.obj :  mouse.c
  $(BCCD32) -P- -c @&&|
 $(CEAT_fdemodexe) $(CNIEAT_fdemodexe) -o$@ mouse.c
|

fdemo.obj :  fdemo.c
  $(BCCD32) -P- -c @&&|
 $(CEAT_fdemodexe) $(CNIEAT_fdemodexe) -o$@ fdemo.c
|

# Compiler configuration file
BccD32.cfg : 
   Copy &&|
-R
-v
-vi
-H
-H=fdemo.csm
-WX
-w-aus
-w-par
-Ot
-K-
-y
-R-
-k
-H-
-H=
-w-pin
| $@


