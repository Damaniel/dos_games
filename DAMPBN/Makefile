CC=gcc
CFLAGS=-O2
DEPS=include/dampbn.h include/palette.h include/uiconsts.h include/render.h include/input.h include/util.h
LIBS=-lalleg

all: dampbn

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

dampbn: src/dampbn.o src/input.o src/render.o src/palette.o src/util.o
	$(CC) -o dampbn.exe src/dampbn.o src/input.o src/render.o src/palette.o src/util.o $(LIBS)

convert: tools/convert.o
	$(CC) -o tools/convert.exe tools/convert.o $(LIBS)

prod: dampbn
	strip dampbn.exe
	upx dampbn.exe

clean: 
	rm -f *.exe src/*.o tools/*.o tools/*.exe
