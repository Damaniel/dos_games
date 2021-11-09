CC=gcc
CFLAGS=-O2
DEPS=inc/dampbn.h inc/palette.h inc/uiconsts.h
LIBS=-lalleg

all: dampbn

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

dampbn: src/dampbn.o src/input.o
	$(CC) -o dampbn.exe src/dampbn.o src/input.o $(LIBS)

convert: tools/convert.o
	$(CC) -o tools/convert.exe tools/convert.o $(LIBS)

prod: dampbn
	strip dampbn.exe
	upx dampbn.exe

clean: 
	rm -f *.exe src/*.o tools/*.o tools/*.exe
