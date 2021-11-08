# Damaniel's Paint By Number Game

## (I don't actually have a name for it yet.)

This will eventually be a color by number game.  Think something like the mobile game
Cross Stitch World, or something like actual diamond painting.  Right now, there's
just a couple mockups, some basic art, a file format specification and a tool to turn
PCX images into files described in the file format doc.  

### Requirements

- A 386 or higher (it runs in protected mode)
- Some amount of RAM.  4MB is probably enough.  
- A VGA that can do 320x240 Mode X resolution.  They pretty much all can.
- A copy of CWSDPMI.EXE, either in the program directory or on your PATH.

### Build requirements

The only component right now is the converter tool, but everything will use the same
toolchain.  Said toolchain is very specific (and old!) though - I'll probably toss a 
pre-assembled DJGPP installation on archive.org somewhere.  It includes:

- DJGPP 2.03
- gcc 2.95.2
- Allegro 3.12

Why am I using these?  They hit the sweet spot between functional and bloated.  
Allegro 4.x does way more than I need, and new versions of gcc generate very large
amounts of code.  Who needs modern standards anyway?

