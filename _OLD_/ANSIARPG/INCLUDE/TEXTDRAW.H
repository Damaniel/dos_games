// Copyright 2022 Shaun Brandt
//   
// Permission is hereby granted, free of charge, to any person obtaining a 
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
// DEALINGS IN THE SOFTWARE.
#ifndef __TEXTDRAW_H__
#define __TEXTDRAW_H__

#include <dos.h>
#include <string.h>

#define CARD_CGA 0
#define CARD_EGA 1
#define CARD_VGA 2

#define LINES_EGA    43
#define LINES_VGA    50

#define BORDER_SINGLE 0
#define BORDER_DOUBLE 1

#define COLOR_BLACK         0x00
#define COLOR_BLUE          0x01
#define COLOR_GREEN         0x02
#define COLOR_CYAN          0x03
#define COLOR_RED           0x04
#define COLOR_MAGENTA       0x05
#define COLOR_BROWN         0x06
#define COLOR_LIGHT_GRAY    0x07
#define COLOR_DARK_GRAY     0x08
#define COLOR_LIGHT_BLUE    0x09
#define COLOR_LIGHT_GREEN   0x0A
#define COLOR_LIGHT_CYAN    0x0B
#define COLOR_LIGHT_RED     0x0C
#define COLOR_LIGHT_MAGENTA 0x0D
#define COLOR_YELLOW        0x0E
#define COLOR_WHITE         0x0F

// The available text modes.  Both EGA and VGA have a 25 line mode.
// The EGA has a 43 line text mode, and the VGA has a 50 line mode.  
// MODE_80X50 will set either the 43 or 50 line mode depending on the 
// video card available.
#define MODE_80X25             0
#define MODE_80X50             1

typedef struct {
  char x1;
  char y1;
  char x2;
  char y2;
  char w;
  char h;
} Rect;

extern char far *g_screen;

//-----------------------------------------------------------------------------
// set_bg_intensity
//
// Toggles the bit in the EGA/VGA that determines whether the high bit
// of the attribute byte serves as a 'blink' bit or as an intensity bit.
//-----------------------------------------------------------------------------
extern void set_bg_intensity(int enabled);


//-----------------------------------------------------------------------------
// set_rect
//
// Sets the 4 extents of the provided rectangle.  Also calculates the width
// and height of the region and stores it.
//-----------------------------------------------------------------------------
extern void set_rect(Rect *r, int x, int y, int w, int h);

//-----------------------------------------------------------------------------
// make_attr
//
// Creates an 8 bit char that contains an attribute byte - that is,
// a foreground and background color that define how a particular character on
// the screen will be displayed. 
//-----------------------------------------------------------------------------
extern char make_attr(int fg, int bg);


//-----------------------------------------------------------------------------
// clear_screen
//
// Clears the screen.  Specifically, blanks out the character bytes and sets
// the attribute bytes to 'gray foreground, black background) that DOS uses
// by default.
//-----------------------------------------------------------------------------
extern void clear_screen(void);

//-----------------------------------------------------------------------------
// detect_adapter
//
// Determines whether the program is running on a CGA or older, EGA, or VGA.
//-----------------------------------------------------------------------------
extern int detect_adapter(void);

//-----------------------------------------------------------------------------
// hide_cursor / show_cursor
//
// Hides or shows the cursor.  That's all.
//-----------------------------------------------------------------------------
extern void hide_cursor(void);
extern void show_cursor(void);

//-----------------------------------------------------------------------------
// set_cursor_at
//
// Moves the cursor to the specified x/y position
//-----------------------------------------------------------------------------
extern void set_cursor_at(int x, int y);

//-----------------------------------------------------------------------------
// set_text_mode
//
// Sets either a 25 line, or a 43/50 line text mode, depending on the display
// adapter (43 for EGA, 50 for VGA)
//-----------------------------------------------------------------------------
extern void set_text_mode(int mode);

//-----------------------------------------------------------------------------
// char_at
// 
// Places a character/attribute pair at the specified location.
//-----------------------------------------------------------------------------
extern void char_at(int x, int y, char c, char attr);

//-----------------------------------------------------------------------------
// hline_at / vline_at
// 
// Places a row or column of character/attribute pairs starting at the 
// specified x and y position, of the specified length.
//-----------------------------------------------------------------------------
extern void hline_at(int x, int y, int len, char c, char attr);
extern void vline_at(int x, int y, int len, char c, char attr);

//-----------------------------------------------------------------------------
// string_at
// 
// Draws a string with the specified attribute at the specified location.
//-----------------------------------------------------------------------------
extern void string_at(int x, int y, char *str, char attr);

//-----------------------------------------------------------------------------
// box_at
// 
// Draws a single or double lined box with the corners defined by 
// x1, x2, y1 and y2.
//-----------------------------------------------------------------------------
extern void box_at(int x1, int y1, int x2, int y2, int border_type, char attr);

//-----------------------------------------------------------------------------
// box_at_rect
// 
// Draws a single or double lined box with the corners defined by the provided
// Rect.
//-----------------------------------------------------------------------------
extern void box_at_rect(Rect r, int border_type, char attr);

//-----------------------------------------------------------------------------
// fill_box_at
// 
// Draws a filled box consisting of a single char/attribute pair,  with the 
// corners defined by x1, x2, y1 and y2.
//-----------------------------------------------------------------------------
extern void fill_box_at(int x1, int y1, int x2, int y2, char c, char attr);

//-----------------------------------------------------------------------------
// fill_box_rect
// 
// Draws a filled box consisting of a single char/attribute pair,  with the 
// corners defined by the specified Rect.
//-----------------------------------------------------------------------------
extern void fill_box_at_rect(Rect r, char c, char attr);

#endif
