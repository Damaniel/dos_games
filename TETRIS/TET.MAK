ORIGIN		= Symantec C++
ORIGIN_VER	= Version 7.20
VERSION		= RELEASE

!IFDEF SUB_DEBUG
DEBUG		= $(SUB_DEBUG)
NDEBUG		= !$(SUB_DEBUG)
!ELSE
DEBUG		= 0
NDEBUG		= 1
!ENDIF

PROJ		= tet
APPTYPE		= DOS EXE
PROJTYPE	= EXE

CC		= SC
CPP		= SPP
MAKE		= SMAKE
RC		= RCC
HC		= HC31
ASM		= SC
DISASM		= OBJ2ASM
LNK		= LINK
DLLS		= 

HEADERS		= ..\source\day_03\graph3.h ..\source\day_04\graph4.h ..\source\day_07\graph7k.h  \
		..\SC\INCLUDE\stdio.h ..\SC\INCLUDE\stdlib.h ..\SC\INCLUDE\time.h ..\SC\INCLUDE\string.h  \
		..\SC\INCLUDE\io.h ..\SC\INCLUDE\conio.h ..\SC\INCLUDE\dos.h ..\SC\INCLUDE\bios.h  \
		..\SC\INCLUDE\memory.h ..\SC\INCLUDE\malloc.h ..\SC\INCLUDE\math.h ..\SC\INCLUDE\fcntl.h  \
		..\source\DAY_04\graph3.h 

DEFFILE		= tet.DEF

!IF $(DEBUG)
OUTPUTDIR	= .
CREATEOUTPUTDIR	=
TARGETDIR	= .
CREATETARGETDIR	=

LIBS		= 

CFLAGS		=  -ms -C -S -3 -a2 -c -g 
CFLAGS_1	=  -ms -C -S -3 -a2 -c -g
DEFINES_1	= 
LFLAGS		=  /CO /DE /XN
DEFINES		= 
!ELSE
OUTPUTDIR	= .
CREATEOUTPUTDIR	=
TARGETDIR	= .
CREATETARGETDIR	=

LIBS		= 

CFLAGS		=  -cpp -Jm -HX -Ae -ml -3 -a2 -c -x 
CFLAGS_1	=  -cpp -Jm -HX -Ae -ml -3 -a2 -c -x
DEFINES_1	= 
LFLAGS		=  /DE /PACKF /XN
DEFINES		= 
!ENDIF

HFLAGS		= $(CFLAGS) 
MFLAGS		= MASTERPROJ=$(PROJ) 
LIBFLAGS	=  /C 
RESFLAGS	=  -32 
DEBUGGERFLAGS	=  
AFLAGS		= $(CFLAGS) 
HELPFLAGS	= 

MODEL		= L

PAR		= PROJS BATS OBJS

RCDEFINES	= 

INCLUDES	= 

INCLUDEDOBJS	= 

OBJS		= ..\SOURCE\DAY_03\GRAPH3.OBJ ..\source\DAY_04\GRAPH4.OBJ ..\SOURCE\DAY_07\GRAPH7K.OBJ  \
		tet.OBJ 

RCFILES		= 

RESFILES	= 

SYMS		= 

HELPFILES	= 

BATS		= 

.SUFFIXES: .C .CP .CPP .CXX .CC .H .HPP .HXX .COM .EXE .DLL .LIB .RTF .DLG .ASM .RES .RC .OBJ 

.C.OBJ:
	$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -o$*.obj $*.c

.CPP.OBJ:
	$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -o$*.obj $*.cpp

.CXX.OBJ:
	$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -o$*.obj $*.cxx

.CC.OBJ:
	$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -o$*.obj $*.cc

.CP.OBJ:
	$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -o$*.obj $*.cp

.H.SYM:
	$(CC) $(HFLAGS) $(DEFINES) $(INCLUDES) -HF -o.\$(*B).sym $*.h

.HPP.SYM:
	$(CC) $(HFLAGS) $(DEFINES) $(INCLUDES) -HF -o.\$(*B).sym $*.hpp

.HXX.SYM:
	$(CC) $(HFLAGS) $(DEFINES) $(INCLUDES) -HF -o.\$(*B).sym $*.hxx

.C.EXP:
	$(CPP) $(CFLAGS) $(DEFINES) $(INCLUDES)   $*.c   -o$*.lst

.CPP.EXP:
	$(CPP) $(CFLAGS) $(DEFINES) $(INCLUDES) $*.cpp -o$*.lst

.CXX.EXP:
	$(CPP) $(CFLAGS) $(DEFINES) $(INCLUDES) $*.cxx -o$*.lst

.CP.EXP:
	$(CPP) $(CFLAGS) $(DEFINES) $(INCLUDES)  $*.cp  -o$*.lst

.CC.EXP:
	$(CPP) $(CFLAGS) $(DEFINES) $(INCLUDES)  $*.cc  -o$*.lst

.ASM.EXP:
	$(CPP) $(CFLAGS) $(DEFINES) $(INCLUDES) $*.asm -o$*.lst

.OBJ.COD:
	$(DISASM) $*.OBJ -c

.OBJ.EXE:
	$(LNK) $(LFLAGS) @$(PROJ).LNK

.RTF.HLP:
	$(HC) $(HELPFLAGS) $*.HPJ

.ASM.OBJ:
	$(ASM) $(AFLAGS) $(DEFINES) $(INCLUDES) -o$*.obj $*.asm

.RC.RES: 
	$(RC) $(RCDEFINES) $(RESFLAGS) $(INCLUDES) $*.rc -o$*.res

.DLG.RES:
	echo ^#include "windows.h" >$$$*.rc
	echo ^IF EXIST "$*.h" >>$$$*.rc
	echo ^#include "$*.h" >>$$$*.rc
	echo ^#include "$*.dlg" >>$$$*.rc
	$(RC) $(RCDEFINES) $(RESFLAGS) $(INCLUDES) $$$*.rc
	-del $*.res
	-ren $$$*.res $*.res



all:	createdir $(PRECOMPILE) $(SYMS) $(OBJS) $(INCLUDEDOBJS) $(POSTCOMPILE) $(TARGETDIR)\$(PROJ).$(PROJTYPE) $(POSTLINK) _done

createdir:
	$(CREATEOUTPUTDIR)
	$(CREATETARGETDIR)

$(TARGETDIR)\$(PROJ).$(PROJTYPE): $(OBJS) $(INCLUDEDOBJS) $(RCFILES) $(RESFILES) $(HELPFILES) 
			-del $(TARGETDIR)\$(PROJ).$(PROJTYPE)
			$(LNK) $(LFLAGS) @$(PROJ).LNK;
			-ren $(TARGETDIR)\$$SCW$$.$(PROJTYPE) $(PROJ).$(PROJTYPE)
			-echo $(TARGETDIR)\$(PROJ).$(PROJTYPE) built

_done:
		-echo $(PROJ).$(PROJTYPE) done

buildall:	clean	all


clean:
		-del $(TARGETDIR)\$$SCW$$.$(PROJTYPE)
		-del $(TARGETDIR)\$(PROJ).CLE
		-del $(OUTPUTDIR)\SCPH.SYM
		-del tet.dpd
		-del $(OBJS)

cleanres:

res:		cleanres $(RCFILES) all


link:
		$(LNK) $(LFLAGS) @$(PROJ).LNK;
		-del $(TARGETDIR)\$(PROJ).$(PROJTYPE)
		-ren $(TARGETDIR)\$$SCW$$.$(PROJTYPE) $(PROJ).$(PROJTYPE)




!IF EXIST (tet.dpd)
!INCLUDE tet.dpd
!ENDIF

tet.OBJ:	tet.cpp
		$(CC) $(CFLAGS_1) $(DEFINES_1) $(INCLUDES) -otet.obj tet.cpp




