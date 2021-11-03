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

typedef struct {
  int w;
  int h;
  char *buffer;
  char *palette;
} Bitmap;

/* 
 * A sprite structure:
 * 
 * Fields:
 *  - width: the width of the sprite
 *  - height: the height of the sprite
 *  - numframes: number of animation frames
 *  - curframe: the current animation frame
 *  - animspeed: number of update frames each sprite frame is displayed
 *  - animcounter: number of update frames that the current frame has
 *    been displayed
 *  - behind: the part of the screen behind the currently drawn sprite
 *    (so it can be restored when the sprite is moved)
 *  - spritesheet: contains all frames of the animation in a single row. 
 *    It must have dimensions greater than or equal to 
 *    (numframes*width) x height
 */
typedef struct {
  int width;
  int height;
  int numframes;
  int curframe;
  int animspeed;
  int animcounter;
  Bitmap *behind;
  Bitmap *spritesheet;
} Sprite;

typedef char Palette;

/*
 * screen - a Bitmap reference to the Mode 13 framebuffer 
 */
Bitmap *screen;

/*
 * sysfont - a pointer to the text mode character set.
 */
char *sysfont;

/*
 * palette - an array of palette data.  Used as palette values to set when
 * using setpalette, and the destination of data received by calling
 * getpalette.
 */
Palette *palette;

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
void setpixel(Bitmap *dest, int x, int y, int color);

/*============================================================================
 * getpixel
 *
 * Retreives the color at the specified location in the specified buffer.
 *==========================================================================*/
int getpixel(Bitmap *dest, int x, int y);

/*============================================================================
 * hline
 *
 * Draws a horizontal line from point x1,y to x2,y in the specified color.
 *==========================================================================*/
void hline(Bitmap *dest, int x1, int x2, int y, int color);

/*============================================================================
 * vline
 *
 * Draws a vertical line from point x,y1 to x,y2 in the specified color.
 *==========================================================================*/
void vline(Bitmap *dest, int x, int y1, int y2, int color);

/*============================================================================
 * drawsysfontchar
 *
 * Draws an ASCII character in the specified location using the system font.  
 *==========================================================================*/
void drawsysfontchar(Bitmap *dest, int xc, int yc, char c, int color, int trans);

/*============================================================================
 * drawsysfontstring
 *
 * Draws a string of ASCII characters in the specified location using the
 * system font.
 *==========================================================================*/
void drawsysfontstring(Bitmap *dest, int x, int y, char *text, int color, int trans);

/*============================================================================
 * getpalette
 *
 * Retrieves the current VGA palette and places it into the specified buffer.
 *==========================================================================*/
void getpalette(Palette *dest);

/*============================================================================
 * setpalette
 *
 * Sets the VGA palette using the values in the specified buffer.
 *==========================================================================*/
void setpalette(Palette *src);

/*============================================================================
 * getcolor
 *
 * Gets the RGB triplet for the specified index of the specified palette.
 *==========================================================================*/
Color getcolor(Palette *pal, int index);

/*============================================================================
 * setcolor
 *
 * Sets the specified entry of the specified palette to the provided RGB
 * color.
 *==========================================================================*/
void setcolor(Palette *pal, int index, Color c);

/*============================================================================
 * clear
 *
 * Clears the specified buffer to the specified color.
 *==========================================================================*/
void clear(Bitmap *dest, int color);

/*============================================================================
 * createbitmap
 *
 * Creates a bitmap of the specified size.  If p is not NULL, then
 * the palette of the Bitmap object will be set to the contents of p.
 *
 * Bitmaps created with createbitmap() need to be freed when they are no 
 * longer needed; use freebitmap() for that.
 *==========================================================================*/
Bitmap *createbitmap(int w, int h, Palette *p);

/*============================================================================
 * blitbitmap
 *
 * A convenience function.  Blits the entire source bitmap to the destination
 * location, clipping as needed.
 *==========================================================================*/
void blitbitmap(Bitmap *dest, Bitmap *src, int x, int y);

/*============================================================================
 * clippedblit
 *
 * Blits the specified source region (position, width, height) to the 
 * destination location, clipping the edges of the source if required.
 * Note that the source region specified must be fully within the source
 * bitmap, but any part of it can fall outside the destination bitmap. 
 *==========================================================================*/
void clippedblit(Bitmap *dest, Bitmap *src, int srcx, int srcy, 
                 int destx, int desty, int w, int h) ;
                 
/*============================================================================
 * clippedblit
 *
 * Blits the specified source region (position, width, height) to the 
 * destination location. No clipping or bounds checking is done, so bad things
 * will happen if your source or destination bitmaps reference locations
 * outside of their bounds.  Useful if you know that your source region
 * only references pixels inside its own region and will fit fully within
 * the bounds of the destination.
 *==========================================================================*/
void blit(Bitmap *dest, Bitmap *src, int srcx, int srcy, int destx, 
              int desty, int w, int h);
             
/*============================================================================
 * freebitmap
 *
 * Deallocates memory used by a Bitmap.
 *==========================================================================*/
void freebitmap(Bitmap *b);

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
