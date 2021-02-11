bcc -Iinclude -nbin -edungeon.exe -ml -O2 src\dungeon.cpp src\stateffs.cpp src\player.cpp src\main.cpp
del bin\*.obj