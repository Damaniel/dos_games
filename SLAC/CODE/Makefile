#=================================================================================
# Makefile for SLAC.  Requires the following:
# - A functional installation of DJGPP
# - A copy of Allegro
#
# In my case, I'm using very particular retro versions:
# - gcc 2.95.2
# - Allegro 3.0
#
# Your mileage may vary if you use later versions of DJGPP and/or Allegro.
# Replacing the former is possibly OK, the latter probably not.
#
# Also note - this Makefile requires that rm.exe (from the DJGPP fileutils package)
# be installed.  This isn't normally a required package for a functional DJGPP
# distribution, but the package is available in the v2gnu/ directory.
#==================================================================================
SRCDIR=src
INCLUDEDIR=inc
OBJDIR=obj

CC=gpp
CFLAGS=-O2 -I$(INCLUDEDIR)
LIBS=-lalleg -lm

TARGET=slac.exe

_DEPS=render.h dat.h globals.h maze.h
DEPS=$(addprefix $(INCLUDEDIR)/, $(_DEPS))

_OBJS=main.o render.o maze.o
OBJS=$(addprefix $(OBJDIR)/, $(_OBJS))

$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(LIBS)
	
prod: $(TARGET) 
	strip $(TARGET)
	upx $(TARGET)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp $(DEPS)
	$(CC) -c $(CFLAGS) -o $@ $<
	
clean:
	@echo Removing files...
	@rm -f $(OBJDIR)/*.o
	@rm -f $(TARGET)
	