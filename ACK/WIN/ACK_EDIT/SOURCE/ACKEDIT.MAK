#
# Borland C++ IDE generated makefile
#
.AUTODEPEND


#
# Borland C++ tools
#
IMPLIB  = Implib
BCC32   = Bcc32 +BccW32.cfg 
TLINK32 = TLink32
TLIB    = TLib
BRC32   = Brc32
TASM32  = Tasm32
#
# IDE macros
#


#
# Options
#
IDE_LFLAGS32 =  -L\bc4\lib
IDE_RFLAGS32 = 
LLATW32_ackeditdexe =  -Tpe -aa -c
RLATW32_ackeditdexe =  -w32
BLATW32_ackeditdexe = 
CNIEAT_ackeditdexe = -I\BC4\INCLUDE;\MSVC\ACK -D_RTLDLL;_BIDSDLL;_OWLDLL;_OWLPCH;
LNIEAT_ackeditdexe = -x
LEAT_ackeditdexe = $(LLATW32_ackeditdexe)
REAT_ackeditdexe = $(RLATW32_ackeditdexe)
BEAT_ackeditdexe = $(BLATW32_ackeditdexe)
CLATW16_acklibdlib = 
LLATW16_acklibdlib = 
RLATW16_acklibdlib = 
BLATW16_acklibdlib = 
CEAT_acklibdlib = $(CEAT_ackeditdexe) $(CLATW16_acklibdlib)
CNIEAT_acklibdlib = -I\BC4\INCLUDE;\MSVC\ACK -D_RTLDLL;_BIDSDLL;_OWLDLL;_OWLPCH;
LNIEAT_acklibdlib = -x
LEAT_acklibdlib = $(LEAT_ackeditdexe) $(LLATW16_acklibdlib)
REAT_acklibdlib = $(REAT_ackeditdexe) $(RLATW16_acklibdlib)
BEAT_acklibdlib = $(BEAT_ackeditdexe) $(BLATW16_acklibdlib)

#
# Dependency List
#
Dep_ackedit = \
   ackedit.exe

ackedit : BccW32.cfg $(Dep_ackedit)
  echo MakeNode 

Dep_ackeditdexe = \
   ackedit.obj\
   acksup.obj\
   ackdlg.obj\
   wingdll.obj\
   winpal.obj\
   acklib.lib\
   ackedit.res

ackedit.exe : $(Dep_ackeditdexe)
  $(TLINK32) @&&|
 /v $(IDE_LFLAGS32) $(LEAT_ackeditdexe) $(LNIEAT_ackeditdexe) +
\bc4\lib\c0w32.obj+
ackedit.obj+
acksup.obj+
ackdlg.obj+
wingdll.obj+
winpal.obj
$<,$*
acklib.lib+
\bc4\lib\bidsfi.lib+
\bc4\lib\owlwfi.lib+
\bc4\lib\import32.lib+
\bc4\lib\cw32i.lib

|
   $(BRC32) ackedit.res $<

ackedit.obj :  ackedit.cpp
  $(BCC32) -c @&&|
 $(CEAT_ackeditdexe) $(CNIEAT_ackeditdexe) -o$@ ackedit.cpp
|

acksup.obj :  acksup.cpp
  $(BCC32) -c @&&|
 $(CEAT_ackeditdexe) $(CNIEAT_ackeditdexe) -o$@ acksup.cpp
|

ackdlg.obj :  ackdlg.cpp
  $(BCC32) -c @&&|
 $(CEAT_ackeditdexe) $(CNIEAT_ackeditdexe) -o$@ ackdlg.cpp
|

wingdll.obj :  wingdll.cpp
  $(BCC32) -c @&&|
 $(CEAT_ackeditdexe) $(CNIEAT_ackeditdexe) -o$@ wingdll.cpp
|

winpal.obj :  winpal.cpp
  $(BCC32) -c @&&|
 $(CEAT_ackeditdexe) $(CNIEAT_ackeditdexe) -o$@ winpal.cpp
|

ackedit.res :  ackedit.rc
  $(BRC32) $(IDE_RFLAGS32) $(REAT_ackeditdexe) $(CNIEAT_ackeditdexe) -R -FO$@ ackedit.rc

# Compiler configuration file
BccW32.cfg : 
   Copy &&|
-R
-v
-vi
-H
-H=ackedit.csm
-W
-H"owl\owlpch.h"
-Ot
| $@


