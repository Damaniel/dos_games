gcc -O2 -o convert.exe convert.c -lalleg
strip convert.exe
upx convert.exe