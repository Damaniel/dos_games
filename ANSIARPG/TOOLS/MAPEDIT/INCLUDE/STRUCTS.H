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
#ifndef __STRUCTS_H__
#define __STRUCTS_H__

typedef struct {
    unsigned char cursor_x;          // position of the cursor in the map area
    unsigned char cursor_y;          
    unsigned char old_cursor_x;      // The previous position of the cursor.  Used to
    unsigned char old_cursor_y;      // remove the old cursor when moving
    unsigned char map_x;             // position of the map at the upper left corner of
    unsigned char map_y;             // the draw area
    unsigned char palette_entry;
    unsigned char exit_entry;    
    unsigned char quit;              // Quit the program?
} AppConfig;

typedef struct {
    char background_attr;
    char menu_attr;
    char highlight_attr;
} UIConfig;

typedef enum {
    PI_NAME,
    PI_FOREGROUND,
    PI_BACKGROUND,
    PI_CHARACTER,
    PI_SOLID,
    PI_DAMAGE,
    PI_OK,
    PI_CANCEL
} PaletteMenuItem;

typedef enum {
    DT_NONE,
    DT_LOW,
    DT_MEDIUM,
    DT_HIGH,
    DT_OHKO
} DamageType;

typedef struct {
    PaletteMenuItem active_item;
    unsigned char name_idx;
    char name[9];
    unsigned char foreground;
    unsigned char background;
    unsigned char character;
    unsigned char solid;
    DamageType damage_type;
} PaletteMenuConfig;

typedef struct {
    unsigned char id;
    char name[9];
    unsigned char glyph;
    unsigned char fg;
    unsigned char bg;
    unsigned char flags1;
    unsigned char flags2;
} PaletteEntry;

typedef struct {
    unsigned char is_set;
    unsigned char target_room;
    unsigned char x_pos;
    unsigned char y_pos;
} Exit;

typedef enum {
    MAIN_SCREEN,
    EXIT_EDIT,
    PALETTE_EDIT
} State;

#endif
