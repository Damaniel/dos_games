bcc -Iinclude -nbin -edungeon.exe -ml -O2 src\dungeon.cpp src\player.cpp src\main.cpp  graphics.lib
del bin\*.obj