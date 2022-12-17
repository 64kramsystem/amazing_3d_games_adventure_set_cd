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
IDE_LFLAGS =  -LE:\BC4\LIB
IDE_RFLAGS = 
LLATW16_tmacompdexe =  -Twe -c -C -LC:\BC45\LIB;E:\BC45\LIB
RLATW16_tmacompdexe =  -31
BLATW16_tmacompdexe = 
CNIEAT_tmacompdexe = -IC:\BC45\INCLUDE;E:\BC45\INCLUDE -D
LNIEAT_tmacompdexe = -s
LEAT_tmacompdexe = $(LLATW16_tmacompdexe)
REAT_tmacompdexe = $(RLATW16_tmacompdexe)
BEAT_tmacompdexe = $(BLATW16_tmacompdexe)

#
# Dependency List
#
Dep_tmacomp = \
   tmacomp.exe

tmacomp : BccW16.cfg $(Dep_tmacomp)
  echo MakeNode 

Dep_tmacompdexe = \
   tmacomp.obj\
   tmacomp.def

tmacomp.exe : $(Dep_tmacompdexe)
  $(TLINK)   @&&|
 /v $(IDE_LFLAGS) $(LEAT_tmacompdexe) $(LNIEAT_tmacompdexe) +
E:\BC45\LIB\c0wl.obj+
tmacomp.obj
$<,$*
E:\BC45\LIB\import.lib+
E:\BC45\LIB\mathwl.lib+
E:\BC45\LIB\cwl.lib
tmacomp.def
|

tmacomp.obj :  tmacomp.cpp
  $(BCC)   -c @&&|
 $(CEAT_tmacompdexe) $(CNIEAT_tmacompdexe) -o$@ tmacomp.cpp
|

# Compiler configuration file
BccW16.cfg : 
   Copy &&|
-R
-v
-vi
-H
-H=tmacomp.csm
-3
-WE
-ml
-WS
-v-
-R-
-k-
| $@


