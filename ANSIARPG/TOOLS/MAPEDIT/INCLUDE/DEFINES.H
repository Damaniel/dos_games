/* Copyright 2022 Shaun Brandt
   
   Permission is hereby granted, free of charge, to any person obtaining a 
   copy of this software and associated documentation files (the "Software"),
   to deal in the Software without restriction, including without limitation
   the rights to use, copy, modify, merge, publish, distribute, sublicense,
   and/or sell copies of the Software, and to permit persons to whom the
   Software is furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included
   in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
   DEALINGS IN THE SOFTWARE.
 */
#ifndef __DEFINES_H__
#define __DEFINES_H__

#define MAP_WIDTH              150
#define MAP_HEIGHT             150

// The position and size of the drawable map area
#define MAP_AREA_X               1
#define MAP_AREA_Y               2

#define MAP_AREA_WIDTH          61
#define MAP_AREA_HEIGHT         22

// The minimum and maximum positions of the map cursor on screen
#define MIN_SCREEN_CURSOR_X            MAP_AREA_X
#define MIN_SCREEN_CURSOR_Y            MAP_AREA_Y
#define MAX_SCREEN_CURSOR_X            (MAP_AREA_X + MAP_AREA_WIDTH - 1)
#define MAX_SCREEN_CURSOR_Y            (MAP_AREA_Y + MAP_AREA_HEIGHT - 1)

// The minimum and maximum positions of the top left of the map on the screen
// (used to ensure that the map never displays area outside of the map array)
#define MIN_SCREEN_MAP_X                0
#define MIN_SCREEN_MAP_Y                0
#define MAX_SCREEN_MAP_X                (MAP_WIDTH - MAP_AREA_WIDTH)
#define MAX_SCREEN_MAP_Y                (MAP_HEIGHT - MAP_AREA_HEIGHT)

#define PALETTE_AREA_X          63
#define PALETTE_AREA_Y           2

// Palette 0 can't be redefined.  
#define NUM_PALETTE_ENTRIES     16
#define MIN_PALETTE_ENTRY        1
#define MAX_PALETTE_ENTRY       NUM_PALETTE_ENTRIES - 1

#define NUM_EXITS                5

#define PALETTE_EDIT_X          20
#define PALETTE_EDIT_Y           6

#define PALETTE_EDIT_WIDTH      24
#define PALETTE_EDIT_HEIGHT     14

#define PALETTE_ITEM_NAME_LENGTH    8

// Flag 1 bits
// bit 0     solid (1) / passable (0)
#define FLAG_SOLID_MASK               0x01
#define FLAG_PASSABLE                 0x00
#define FLAG_SOLID                    0x01
// bits 1-3  floor damage types (000 - none, 001 - low, 010 - medium, 011 - high, 100 - OHKO)
#define FLAG_DAMAGE_MASK              0x0E
#define FLAG_DAMAGE_NONE              0x00
#define FLAG_DAMAGE_LOW               0x01
#define FLAG_DAMAGE_MEDIUM            0x02
#define FLAG_DAMAGE_HIGH              0x03
#define FLAG_DAMAGE_OHKO              0x04

#endif
