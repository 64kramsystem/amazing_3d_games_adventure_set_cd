#
# Borland C++ IDE generated makefile
#
.AUTODEPEND


#
# Borland C++ tools
#
IMPLIB  = Implib
BCC     = Bcc +BccW16.cfg 
TLINK   = TLink
TLIB    = TLib
BRC     = Brc
TASM    = Tasm
#
# IDE macros
#


#
# Options
#
IDE_LFLAGS =  -LE:\BC45\LIB -3
IDE_RFLAGS =  -31 -l0
LW16Default_Project_Options =  -LE:\BC45\LIB -3
RW16Default_Project_Options =  -31 -l0
BW16Default_Project_Options = 
LNATW16_wingtddexe = $(LW16Default_Project_Options)
RNATW16_wingtddexe = $(RW16Default_Project_Options)
BNATW16_wingtddexe = $(BW16Default_Project_Options)
LLATW16_wingtddexe =  -Twe -c -C
RLATW16_wingtddexe =  -31 -k-
BLATW16_wingtddexe = 
CNIEAT_wingtddexe = -IC:\BC45\INCLUDE;C:\WING\INCLUDE -DSTRICT; TILE4
LNIEAT_wingtddexe = -s
LEAT_wingtddexe = $(LNATW16_wingtddexe) $(LLATW16_wingtddexe)
REAT_wingtddexe = $(RNATW16_wingtddexe) $(RLATW16_wingtddexe)
BEAT_wingtddexe = $(BNATW16_wingtddexe) $(BLATW16_wingtddexe)
CLATW16_gen2dasm = 
LLATW16_gen2dasm = 
RLATW16_gen2dasm = 
BLATW16_gen2dasm = 
CEAT_gen2dasm = $(CEAT_wingtddexe) $(CLATW16_gen2dasm)
CNIEAT_gen2dasm = -IC:\BC45\INCLUDE;C:\WING\INCLUDE -DSTRICT; TILE4
LNIEAT_gen2dasm = -s
LEAT_gen2dasm = $(LEAT_wingtddexe) $(LLATW16_gen2dasm)
REAT_gen2dasm = $(REAT_wingtddexe) $(RLATW16_gen2dasm)
BEAT_gen2dasm = $(BEAT_wingtddexe) $(BLATW16_gen2dasm)
CLATW16_mapdcpp = 
LLATW16_mapdcpp = 
RLATW16_mapdcpp = 
BLATW16_mapdcpp = 
CEAT_mapdcpp = $(CEAT_wingtddexe) $(CLATW16_mapdcpp)
CNIEAT_mapdcpp = -IC:\BC45\INCLUDE;C:\WING\INCLUDE -DSTRICT; TILE4
LNIEAT_mapdcpp = -s
LEAT_mapdcpp = $(LEAT_wingtddexe) $(LLATW16_mapdcpp)
REAT_mapdcpp = $(REAT_wingtddexe) $(RLATW16_mapdcpp)
BEAT_mapdcpp = $(BEAT_wingtddexe) $(BLATW16_mapdcpp)
CLATW16_wingbmpdcpp =  -v -N -y -vi-
LLATW16_wingbmpdcpp = 
RLATW16_wingbmpdcpp = 
BLATW16_wingbmpdcpp = 
CEAT_wingbmpdcpp = $(CEAT_wingtddexe) $(CLATW16_wingbmpdcpp)
CNIEAT_wingbmpdcpp = -IC:\BC45\INCLUDE;C:\WING\INCLUDE -DSTRICT; TILE4
LNIEAT_wingbmpdcpp = -s
LEAT_wingbmpdcpp = $(LEAT_wingtddexe) $(LLATW16_wingbmpdcpp)
REAT_wingbmpdcpp = $(REAT_wingtddexe) $(RLATW16_wingbmpdcpp)
BEAT_wingbmpdcpp = $(BEAT_wingtddexe) $(BLATW16_wingbmpdcpp)
CLATW16_wingbmp2dasm = 
LLATW16_wingbmp2dasm = 
RLATW16_wingbmp2dasm = 
BLATW16_wingbmp2dasm = 
CEAT_wingbmp2dasm = $(CEAT_wingtddexe) $(CLATW16_wingbmp2dasm)
CNIEAT_wingbmp2dasm = -IC:\BC45\INCLUDE;C:\WING\INCLUDE -DSTRICT; TILE4
LNIEAT_wingbmp2dasm = -s
LEAT_wingbmp2dasm = $(LEAT_wingtddexe) $(LLATW16_wingbmp2dasm)
REAT_wingbmp2dasm = $(REAT_wingtddexe) $(RLATW16_wingbmp2dasm)
BEAT_wingbmp2dasm = $(BEAT_wingtddexe) $(BLATW16_wingbmp2dasm)
CLATW16_wingdlib = 
LLATW16_wingdlib = 
RLATW16_wingdlib = 
BLATW16_wingdlib = 
CEAT_wingdlib = $(CEAT_wingtddexe) $(CLATW16_wingdlib)
CNIEAT_wingdlib = -IC:\BC45\INCLUDE;C:\WING\INCLUDE -DSTRICT; TILE4
LNIEAT_wingdlib = -s
LEAT_wingdlib = $(LEAT_wingtddexe) $(LLATW16_wingdlib)
REAT_wingdlib = $(REAT_wingtddexe) $(RLATW16_wingdlib)
BEAT_wingdlib = $(BEAT_wingtddexe) $(BLATW16_wingdlib)

#
# Dependency List
#
Dep_wingtd = \
   wingtd.exe

wingtd : BccW16.cfg $(Dep_wingtd)
  echo MakeNode 

Dep_wingtddexe = \
   about.obj\
   bmpinfo.obj\
   debug.obj\
   dibitmap.obj\
   gen.obj\
   gen2.obj\
   logpal.obj\
   map.obj\
   res.obj\
   wingbmp.obj\
   wingbmp2.obj\
   wingdc.obj\
   wingtd.obj\
   wingwnd.obj\
   wingtd.def\
   wing.lib\
   wingtd.res

wingtd.exe : $(Dep_wingtddexe)
  $(TLINK)   @&&|
 /v $(IDE_LFLAGS) $(LEAT_wingtddexe) $(LNIEAT_wingtddexe) +
E:\BC45\LIB\c0wl.obj+
about.obj+
bmpinfo.obj+
debug.obj+
dibitmap.obj+
gen.obj+
gen2.obj+
logpal.obj+
map.obj+
res.obj+
wingbmp.obj+
wingbmp2.obj+
wingdc.obj+
wingtd.obj+
wingwnd.obj
$<,$*
wing.lib+
E:\BC45\LIB\import.lib+
E:\BC45\LIB\mathwl.lib+
E:\BC45\LIB\cwl.lib
wingtd.def
|
   $(BRC) wingtd.res $<

about.obj :  about.cpp
  $(BCC)   -c @&&|
 $(CEAT_wingtddexe) $(CNIEAT_wingtddexe) -o$@ about.cpp
|

bmpinfo.obj :  bmpinfo.cpp
  $(BCC)   -c @&&|
 $(CEAT_wingtddexe) $(CNIEAT_wingtddexe) -o$@ bmpinfo.cpp
|

debug.obj :  debug.cpp
  $(BCC)   -c @&&|
 $(CEAT_wingtddexe) $(CNIEAT_wingtddexe) -o$@ debug.cpp
|

dibitmap.obj :  dibitmap.cpp
  $(BCC)   -c @&&|
 $(CEAT_wingtddexe) $(CNIEAT_wingtddexe) -o$@ dibitmap.cpp
|

gen.obj :  gen.cpp
  $(BCC)   -c @&&|
 $(CEAT_wingtddexe) $(CNIEAT_wingtddexe) -o$@ gen.cpp
|

gen2.obj :  gen2.asm
  $(TASM) @&&|
gen2.asm
|

logpal.obj :  logpal.cpp
  $(BCC)   -c @&&|
 $(CEAT_wingtddexe) $(CNIEAT_wingtddexe) -o$@ logpal.cpp
|

map.obj :  map.cpp
  $(BCC)   -c @&&|
 $(CEAT_mapdcpp) $(CNIEAT_mapdcpp) -o$@ map.cpp
|

res.obj :  res.cpp
  $(BCC)   -c @&&|
 $(CEAT_wingtddexe) $(CNIEAT_wingtddexe) -o$@ res.cpp
|

wingbmp.obj :  wingbmp.cpp
  $(BCC)   -c @&&|
 $(CEAT_wingbmpdcpp) $(CNIEAT_wingbmpdcpp) -o$@ wingbmp.cpp
|

wingbmp2.obj :  wingbmp2.asm
  $(TASM) @&&|
wingbmp2.asm
|

wingdc.obj :  wingdc.cpp
  $(BCC)   -c @&&|
 $(CEAT_wingtddexe) $(CNIEAT_wingtddexe) -o$@ wingdc.cpp
|

wingtd.obj :  wingtd.cpp
  $(BCC)   -c @&&|
 $(CEAT_wingtddexe) $(CNIEAT_wingtddexe) -o$@ wingtd.cpp
|

wingwnd.obj :  wingwnd.cpp
  $(BCC)   -c @&&|
 $(CEAT_wingtddexe) $(CNIEAT_wingtddexe) -o$@ wingwnd.cpp
|

wingtd.res :  wingtd.rc
  $(BRC) $(IDE_RFLAGS) $(REAT_wingtddexe) $(CNIEAT_wingtddexe) -R -FO$@ wingtd.rc

# Compiler configuration file
BccW16.cfg : 
   Copy &&|
-R
-v
-vi
-H
-3
-WE
-y
-N
-vi-
-ml
-wcln
-wsig
-wucp
-wbbf
-wpin
-wnak
-wpre
-wdef
-wnod
-wuse
-wstv
-wasm
-wamp
-wobs
-wpch
-wamb
-3
-WE
-y
-N
-vi-
-ml
-wcln
-wsig
-wucp
-wbbf
-wpin
-wnak
-wpre
-wdef
-wnod
-wuse
-wstv
-wasm
-wamp
-wobs
-wpch
-wamb
-y-
-v
-R
-N-
-vi
-O-c
-Os
-Z-
-O-
-O-e
-O-l
-O-b
-O-W
-Om
-Op
-Oi
-Ov
-H=WINGTD.CSM
-k-
-ml
-WE
| $@


