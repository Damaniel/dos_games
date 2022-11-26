# ANSIARPG (working title)

## About
This will eventually be a Diablo-like game, but using text mode.  The goal isn't
to be like ZZT (which isn't really the same thing), but to be a real-time game
with loot, attacks and skills, leveling up, and all the ARPG things.  

There's also a map editor to create the levels (as they aren't intended to be
randomly generated - at least not yet).  See the relevant section below.

## Building
These tools require Watcom C/C++.  I'm using version 11.0, but any OpenWatcom
version should work fine too.  There are no other external dependencies.

Run 'build.bat' to build the program and tools.

## Tools

### Map Editor
The map editor is a tool that allows the developer to create a map that enemies
can be found in.  The map is saved in a compressed format that can later be
expanded into memory inside the game.  

A map consists of a grid of tiles, representing things like walls, doors, 
paths, water, and so on.  Any map can consist of up to 15 types of tiles (plus a 
default, empty tile), consisting of a foreground/background attribute and
a character.

The maximum size of a map is 150x150.  Expanded in memory, this will consume
22.5k - 1 byte per tile (though, in practice, only the lower 4 bits are used
per tile), and each entry in the map array represents the index of one of the
valid tiles.  

### Map linkage
Each map contains a list of linkages - an eight byte value that represents the
square defined by (x1,y1)-(x2,y2) on the current map, the index of the target map and 
an (x,y) position within the target map.  This helps to ensure that when a map
is left, the player will move to the correct location on the new map.  The
list of linkages is fixed at a size of 5, which means that no map can have
more than 5 entrances/exits.

### Map file format
#### Metadata
  - Magic number (2 bytes)
  - Version (2 bytes)
  - Map ID (2 bytes)
  - Map name (20 bytes)
  - Map width/height / 2 (2 bytes)
  - Compression type (1 byte)
  - Default PC position (2 bytes)
  - 97 bytes for future expansion

##### Notes about metadata
The magic number identies the file.  It's set to 'DH'.

Version is specified as 2 bytes - 1 for major number, 1 for minor number

Map ID is the unique identifier associated with this map.  The game itself will
use some kind of mapping system (filename->ID or metafile->ID) to determine 
which map to load, so this field is mainly informational

Map name is a ASCII representation of the name of the map.  

Map width and height specify the size of the map.  It can support 256x256 
maps, but in practice maps will be smaller.

Compression type: either unpacked (00), unpacked RLE (01), packed (10) or
packed RLE (11).  Unpacked compression stores each map tile in 1 byte, while
packed compression sticks 2 tiles per byte.  RLE will run length encode either
type of packed/unpacked format.  The map editor will store maps in uncompressed
format, but it could auto-calculate the most efficient encoding and use that
instead, code space permitting. 

Default PC position specifies where a player will start if the location wasn't
specified from the exit of another map.  This is helpful if the map either isn't
linked from another map, or if this is the starting map of a region.

#### Tile data:
There are a total of 16 tiles per map (208 bytes total):
  - character ID (1 byte)
  - tile name (8 bytes)
  - foreground color (1 byte)
  - background color (1 byte)
  - flags (2 bytes)

The 'tile name' value is mainly used for the map editor, so the map maker
can remember what the tile is supposed to represent.  

Flags represent general values applicable to a tile type:
  - Damage type (none, low, medium, high)
  - Solid
  - TBD

#### Map data:
  - A run-length encoded, 'compressed' version of the (width*height) map data

#### Compression:
  - Each map location is defined using a 4-bit value (representing the tile
    at the location).  In the map file, adjacent tiles (horizontally) are
    packed into a single byte.  The resulting single bytes are then 
    run-length encoded if the resulting size of the file would be smaller,
    in the form <1 byte length, 1 byte packed data>.  Due to the extra length
    byte, RLE will only likely be of beneift when a large area has the same
    adjacent tile types.  The map compressor will determine the most efficient
    way to store the map and set the compression type byte in the header
    accordingly.

#### Notes:
In the header, the width and height of the map are stored as the width / 2
and height / 2.  Maps are required to have an even width and height, so 
the actual map size is calculated as (header value * 2).  Also note that for
the purposes of the game, the width and height values are always set to the
maximum size of the map area.  This means that the two bytes should be set
to 75/75.  While the size values theoretically support a map size of 510x510
tiles (by setting the size to 255/255), the game will set it to a fixed
value of 150x150 tiles.  The map editor itself will also enforce this size
limit.

