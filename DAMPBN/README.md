# Damaniel's Paint By Number Game

## (but I don't actually have a name for it yet...)

This will eventually be a color by number game.  Think something like the mobile game
Cross Stitch World, or something like actual diamond painting.  Right now, there's
just a couple mockups, some basic art, a file format specification and a tool to turn
PCX images into files described in the file format doc.  

### Requirements

- A 386 or higher (it runs in protected mode)
- Some amount of RAM.  4MB is probably enough.  
- A VGA card.  It uses Mode 13 (320x200, 256 colors), supported by all VGAs.
- A copy of CWSDPMI.EXE, either in the program directory or on your PATH.

### Additional tools

There are 2 tools in the 'tools' directory.  You can build them by running 
`make tools` from the root of the main directory.  These tools are:

- convert: The tool that creates picture files from PCX files.  Run convert.exe
           for usage instructions
- expand: takes a PCX saved in 16 color mode and converts it to a 256 color
          version.  This is important since it allows the creation of picture
          files with fewer than 16 colors while maintaining compatibility with
          Allegro.

### Build requirements

The toolchain used by this game is very specific (and old!) though - I'll probably 
toss a pre-assembled DJGPP installation on archive.org somewhere.  It includes:

- DJGPP 2.03
- gcc 2.95.2
- Allegro 3.12

Why am I using these?  They hit the sweet spot between functional and bloated.  
Allegro 4.x does way more than I need, and new versions of gcc generate very large
amounts of code.  Who needs modern standards anyway?

