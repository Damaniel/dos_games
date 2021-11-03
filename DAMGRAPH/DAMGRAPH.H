/* Copyright 2021 Shaun Brandt
   
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
#ifndef __DAMGRAPH_H__
#define __DAMGRAPH_H__

/*
 * Defined graphics modes
 *
 * MODE13H  - 320x200, 256 color mode
 * TEXTMODE - standard 80x24 text mode
 */
#define MODE13H        0x13
#define TEXTMODE       0x03

#define SCREEN_WIDTH   320
#define SCREEN_HEIGHT  200
#define PALETTE_SIZE   768

/*
 * screen - a pointer to the chunk of VGA memory containing the mode 13h 
 * framebuffer.  Can be used as a destination for most of the drawing
 * functions.
 */
char *screen;

/*
 * sysfont - a pointer to the text mode character set.
 */
char *sysfont;

/*
 * palette - an array of palette data.  Used as palette values to set when
 * using setpalette, and the destination of data received by calling
 * getpalette.
 */
char *palette;

/*
 * Color - an RGB triple.  Used to define palette entries.  
 */
typedef struct {
  int r;
  int g;
  int b;
} Color;

/*
 * Bitmap - a generic image holding struct. 
 */
typedef struct {
  int w;
  int h;
  char *buffer;
  char *palette;
} Bitmap;

/*============================================================================
 * setMode
 *
 * Sets the specified DOS VGA text / graphics mode.  
 *==========================================================================*/
void setmode(int mode);

/*============================================================================
 * setpixel
 *
 * Places a pixel of the specified color in the destination buffer.
 *==========================================================================*/
void setpixel(char *dest, int x, int y, int color);

/*============================================================================
 * getpixel
 *
 * Retreives the color at the specified location in the specified buffer.
 *==========================================================================*/
int getpixel(char *dest, int x, int y);

/*============================================================================
 * hline
 *
 * Draws a horizontal line from point x1,y to x2,y in the specified color.
 *==========================================================================*/
void hline(char *dest, int x1, int x2, int y, int color);

/*============================================================================
 * vline
 *
 * Draws a vertical line from point x,y1 to x,y2 in the specified color.
 *==========================================================================*/
void vline(char *dest, int x, int y1, int y2, int color);

/*============================================================================
 * drawsysfontchar
 *
 * Draws an ASCII character in the specified location using the system font.  
 *==========================================================================*/
void drawsysfontchar(char *dest, int xc, int yc, char c, int color, int trans);

/*============================================================================
 * drawsysfontstring
 *
 * Draws a string of ASCII characters in the specified location using the
 * system font.
 *==========================================================================*/
void drawsysfontstring(char *dest, int x, int y, char *text, int color, int trans);

/*============================================================================
 * getpalette
 *
 * Retrieves the current VGA palette and places it into the specified buffer.
 *==========================================================================*/
void getpalette(char *dest);

/*============================================================================
 * setpalette
 *
 * Sets the VGA palette using the values in the specified buffer.
 *==========================================================================*/
void setpalette(char *src);

/*============================================================================
 * getcolor
 *
 * Gets the RGB triplet for the specified index of the specified palette.
 *==========================================================================*/
Color getcolor(char *pal, int index);

/*============================================================================
 * setcolor
 *
 * Sets the specified entry of the specified palette to the provided RGB
 * color.
 *==========================================================================*/
void setcolor(char *pal, int index, Color c);

/*============================================================================
 * clear
 *
 * Clears the specified buffer to the specified color.
 *==========================================================================*/
void clear(char *dest, int color);

/*============================================================================
 * graphinit
 *
 * Sets up the system to make VGA related calls (mainly unprotecting base
 * memory so we can quickly poke registers and such).
 *==========================================================================*/    
int graphinit(void);

/*============================================================================
 * graphshutdown
 *
 * Frees any allocated resources used by the graphics subsystem
 *==========================================================================*/   
int graphshutdown(void);

#endif /* __DAMGRAPH_H__ */
