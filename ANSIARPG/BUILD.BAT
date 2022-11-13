@ECHO OFF
REM set the build memory model (currently large)
SET MEMMODEL=/ml
REM delete any existing executable
DEL *.EXE
REM change to the source directory
CD SRC
REM build textdraw library
DEL /Q ..\LIBS\TEXTDRAW.LIB
WCC TEXTDRAW.C /q %MEMMODEL% /i..\include
WLIB TEXTDRAW +TEXTDRAW /q
MOVE TEXTDRAW.LIB ..\LIBS
REM build gmae app
WCL MAIN.C /i..\INCLUDE %MEMMODEL% /fe=ANSIARPG.EXE ..\LIBS\TEXTDRAW.LIB
REM clean objects and move main app to base directory
DEL *.OBJ
MOVE ANSIARPG.EXE ..
REM change to the tools directory
CD ..
CD TOOLS
ECHO *** Building tools ***
CD ..

