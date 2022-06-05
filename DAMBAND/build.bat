@echo off
REM Delete outputs
del /Q damband.exe
del /Q src\*.obj
del /Q src\*.map

REM build components
cd src
REM bcc -ml -e..\damband.exe damband.c textdraw.c
bcc -ml -I..\include -e..\damband.exe damband.c maze.c textdraw.c render.c player.c
del *.obj
cd ..

