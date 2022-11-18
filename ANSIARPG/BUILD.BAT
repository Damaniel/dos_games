@ECHO OFF
REM set the build memory model (currently large)
SET MEMMODEL=/ml
REM delete any existing executable
DEL *.EXE
REM change to the source directory
CD SRC
REM build textdraw library
DEL /Q ..\LIBS\TEXTDRAW.LIB
ECHO ********************* Building TEXTDRAW.LIB *************************
WCC TEXTDRAW.C /zq %MEMMODEL% /i..\include
WLIB TEXTDRAW +TEXTDRAW /q
MOVE TEXTDRAW.LIB ..\LIBS
REM build gmae app
ECHO ********************* Building ANSIARPG *************************
WCL MAIN.C /zq /i..\INCLUDE %MEMMODEL% /fe=ANSIARPG.EXE ..\LIBS\TEXTDRAW.LIB
REM clean objects and move main app to base directory
DEL *.OBJ
MOVE ANSIARPG.EXE ..
REM change to the tools directory
CD ..
CD TOOLS
REM build all the tools
ECHO ********************* Building MAPEDIT *************************
CD MAPEDIT
WCL INPUT.C RENDER.C MAPEDIT.C /zq /i..\..\INCLUDE %MEMMODEL% /fe=MAPEDIT.EXE ..\..\LIBS\TEXTDRAW.LIB
DEL *.OBJ
MOVE MAPEDIT.EXE ..\..
REM  MAPEDIT is built, go back to tools dir
CD ..
REM tools are built, go back to base dir
CD ..

