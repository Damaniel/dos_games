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
#include "damgraph.h"
#include "imageops.h"
#include <dpmi.h>
#include <pc.h>
#include <unistd.h>
#include <sys/nearptr.h>

/*----------------------------------------------------------------------------
 * setmode
 *--------------------------------------------------------------------------*/
void setmode(int mode) {
  __dpmi_regs regs;    
  
  memset(&regs, 0, sizeof regs);
  regs.x.ax = mode;
  __dpmi_int(0x10, &regs);
}

/*----------------------------------------------------------------------------
 * setpixel
 *--------------------------------------------------------------------------*/
void setpixel(char *dest, int x, int y, int color) {
  dest[(y<<8) + (y<<6) + x] = color;
}
/*----------------------------------------------------------------------------
 * getpixel
 *--------------------------------------------------------------------------*/
int getpixel(char *dest, int x, int y) {
  return dest[(y<<8) + (y<<6) + x];
}


/*----------------------------------------------------------------------------
 * getpalette
 *--------------------------------------------------------------------------*/
void getpalette(char *dest) {
  int i;
  
  if (dest == NULL)
    return;
  
  outportb(0x3c7, 0);
  for (i=0; i < PALETTE_SIZE; i++)
    dest[i] = inportb(0x3c9);
}

/*----------------------------------------------------------------------------
 * setpalette
 *--------------------------------------------------------------------------*/
void setpalette(char *src) {
  int i;

  if (src == NULL)
    return;
  
  outportb(0x3c8, 0);
  for (i=0; i< PALETTE_SIZE; i++) 
    outportb(0x3c9, src[i]);
}

/*----------------------------------------------------------------------------
 * getcolor
 *--------------------------------------------------------------------------*/
Color getcolor(char *pal, int index) {
  Color c;
  int offset = index * 3;
  c.r = pal[offset];
  c.g = pal[offset + 1];
  c.b = pal[offset + 2];
  return c;
}

/*----------------------------------------------------------------------------
 * setcolor
 *--------------------------------------------------------------------------*/
void setcolor(char *pal, int index, Color c) {
  int offset = index * 3;
  pal[offset] = c.r;
  pal[offset + 1] = c.g;
  pal[offset + 2] = c.b;
}

/*----------------------------------------------------------------------------
 * setcolor
 *--------------------------------------------------------------------------*/
void drawsysfontchar(char *dest, int xc, int yc, char c, int color, int trans) {
  int offset, x, y;
  char *chardata;
  unsigned char bitmask = 0x80;
  
  /* Note: the system font is 8x8 */
  chardata = sysfont + (c * 8);
  offset = (yc * SCREEN_WIDTH) + xc;
  
  /* loop through each line of the character */
  for (y=0; y<8; y++) {
    /* Set the bitmask to the first pixel of the line */
    bitmask = 0x80;
    /* loop through each pixel of the line */
    for (x=0; x<8; x++) {
      /* If this pixel contains part of the character, draw it */
      if((*chardata & bitmask))
        dest[offset+x] = color;
      /* If not, then check if the transparency flag is set.  If not,
         draw the space in index 0.  Otherwise, the non-pixel data
         won't be drawn, leaving the underlying image alone */
      else if (!trans)
          dest[offset+x] = 0;
        
       /* Shift the bitmask one pixel over */
       bitmask = (bitmask >> 1);
    }
    offset += SCREEN_WIDTH;
    chardata++;
  }
}

/*----------------------------------------------------------------------------
 * setcolor
 *--------------------------------------------------------------------------*/
void drawsysfontstring(char *dest, int x, int y, char *text, int color, int trans) {
  
  int i;
  
  for(i = 0; text[i] != 0; i++)
    drawsysfontchar(dest, x+(i*8), y, text[i], color, trans);
}

/*----------------------------------------------------------------------------
 * clear
 *--------------------------------------------------------------------------*/
void clear(char *dest, int color) {
  memset(dest, color, SCREEN_WIDTH * SCREEN_HEIGHT);
}

/*----------------------------------------------------------------------------
 * hline
 *--------------------------------------------------------------------------*/
void hline(char *dest, int x1, int x2, int y, int color) {
  memset(dest + (y * SCREEN_WIDTH + x1), color, x2-x1+1);
}

/*----------------------------------------------------------------------------
 * vline
 *--------------------------------------------------------------------------*/
void vline(char *dest, int x, int y1, int y2, int color) {
  int i;
  int pos = y1 * SCREEN_WIDTH + x;
  for (i=0; i < y2-y1+1; i++) {
    dest[pos] = color;
    pos += SCREEN_WIDTH;
  }
}

/*----------------------------------------------------------------------------
 * graphinit
 *--------------------------------------------------------------------------*/
int graphinit(void) {
  if (!__djgpp_nearptr_enable())
    return -1;

  /* Note - to convert segment:offset to linear:
   * - Take upper 4 hex nibbles, add a 0 to the end (that is, multiply by 16)
   *   add the offset to it
   */
   
  /* Point screen to the start of VGA video memory */
  screen = (char *)0xa0000 + __djgpp_conventional_base;
  /* Point sysfont to the VGA font data */
  sysfont = (char *)0xFFA6E + __djgpp_conventional_base;
  
  /* Allocate space to hold the palette */
  palette = (char *)malloc(PALETTE_SIZE * sizeof(char));  
}

/*----------------------------------------------------------------------------
 * graphshutdown
 *--------------------------------------------------------------------------*/
int graphshutdown(void) {
  
  if(palette)
    free(palette);
}