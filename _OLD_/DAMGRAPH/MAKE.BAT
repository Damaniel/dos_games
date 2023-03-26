gcc -O2 -c imageops.c
gcc -O2 -c damgraph.c
gcc -O2 -c test.c
gcc -o test.exe imageops.o damgraph.o test.o