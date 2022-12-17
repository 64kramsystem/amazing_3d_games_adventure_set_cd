# Microsoft Visual C++ generated build script - Do not modify

PROJ = STATIC16
DEBUG = 0
PROGTYPE = 0
CALLER = 
ARGS = 
DLLS = 
D_RCDEFINES = -d_DEBUG
R_RCDEFINES = -dNDEBUG
ORIGIN = MSVC
ORIGIN_VER = 1.00
PROJPATH = K:\ROBERTH\SRC\WING\STATIC\WIN32\
USEMFC = 0
CC = cl
CPP = cl
CXX = cl
CCREATEPCHFLAG = 
CPPCREATEPCHFLAG = 
CUSEPCHFLAG = 
CPPUSEPCHFLAG = 
FIRSTC = STATIC.C    
FIRSTCPP =             
RC = rc
CFLAGS_D_WEXE = /nologo /G2 /W3 /Zi /AL /Od /D "_DEBUG" /FR /GA /Fd"STATIC.PDB"
CFLAGS_R_WEXE = /nologo /W3 /AL /O1 /D "NDEBUG" /FR /GA 
LFLAGS_D_WEXE = /NOLOGO /NOD /PACKC:61440 /STACK:10240 /ALIGN:16 /ONERROR:NOEXE /CO  
LFLAGS_R_WEXE = /NOLOGO /NOD /PACKC:61440 /STACK:10240 /ALIGN:16 /ONERROR:NOEXE  
LIBS_D_WEXE = oldnames libw llibcew wing commdlg.lib mmsystem.lib olecli.lib olesvr.lib shell.lib 
LIBS_R_WEXE = oldnames libw llibcew wing commdlg.lib mmsystem.lib olecli.lib olesvr.lib shell.lib 
RCFLAGS = /nologo
RESFLAGS = /nologo
RUNFLAGS = 
DEFFILE = STATIC.DEF
OBJS_EXT = 
LIBS_EXT = D:\WING\LIB\WING.LIB 
!if "$(DEBUG)" == "1"
CFLAGS = $(CFLAGS_D_WEXE)
LFLAGS = $(LFLAGS_D_WEXE)
LIBS = $(LIBS_D_WEXE)
MAPFILE = nul
RCDEFINES = $(D_RCDEFINES)
!else
CFLAGS = $(CFLAGS_R_WEXE)
LFLAGS = $(LFLAGS_R_WEXE)
LIBS = $(LIBS_R_WEXE)
MAPFILE = nul
RCDEFINES = $(R_RCDEFINES)
!endif
!if [if exist MSVC.BND del MSVC.BND]
!endif
SBRS = STATIC.SBR


STATIC_DEP = d:\wing\include\wing.h \
	k:\roberth\src\wing\static\win32\static.h


STATIC_RCDEP = k:\roberth\src\wing\static\win32\static.h \
	k:\roberth\src\wing\static\win32\static.ico


WING_DEP = 

all:	$(PROJ).EXE $(PROJ).BSC

STATIC.OBJ:	STATIC.C $(STATIC_DEP)
	$(CC) $(CFLAGS) $(CCREATEPCHFLAG) /c STATIC.C

STATIC.RES:	STATIC.RC $(STATIC_RCDEP)
	$(RC) $(RCFLAGS) $(RCDEFINES) -r STATIC.RC


$(PROJ).EXE::	STATIC.RES

$(PROJ).EXE::	STATIC.OBJ $(OBJS_EXT) $(DEFFILE)
	echo >NUL @<<$(PROJ).CRF
STATIC.OBJ +
$(OBJS_EXT)
$(PROJ).EXE
$(MAPFILE)
c:\msvc\lib\+
c:\msvc\mfc\lib\+
c:\hhsdk\lib\+
d:\wing\lib\+
D:\WING\LIB\WING.LIB+
$(LIBS)
$(DEFFILE);
<<
	link $(LFLAGS) @$(PROJ).CRF
	$(RC) $(RESFLAGS) STATIC.RES $@
	@copy $(PROJ).CRF MSVC.BND

$(PROJ).EXE::	STATIC.RES
	if not exist MSVC.BND 	$(RC) $(RESFLAGS) STATIC.RES $@

run: $(PROJ).EXE
	$(PROJ) $(RUNFLAGS)


$(PROJ).BSC: $(SBRS)
	bscmake @<<
/o$@ $(SBRS)
<<
