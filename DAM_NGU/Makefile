CC=gcc
CFLAGS=-O2 -Wall
DEPS=
LIBS=-lalleg

all: dam_ngu

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

dam_ngu: src/main.o src/render.o src/map_ops.o src/input.o
	$(CC) -o dam_ngu.exe src/main.o src/render.o src/map_ops.o src/input.o $(LIBS)

prod: dam_ngu
	strip dam_ngu.exe


clean: 
	rm -f *.exe src/*.o
