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
void setpixel(Bitmap *dest, int x, int y, int color) {
  dest->buffer[(y<<8) + (y<<6) + x] = color;
}
/*----------------------------------------------------------------------------
 * getpixel
 *--------------------------------------------------------------------------*/
int getpixel(Bitmap *dest, int x, int y) {
  return dest->buffer[(y<<8) + (y<<6) + x];
}


/*----------------------------------------------------------------------------
 * getpalette
 *--------------------------------------------------------------------------*/
void getpalette(Palette *dest) {
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
void setpalette(Palette *src) {
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
Color getcolor(Palette *pal, int index) {
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
void setcolor(Palette *pal, int index, Color c) {
  int offset = index * 3;
  pal[offset] = c.r;
  pal[offset + 1] = c.g;
  pal[offset + 2] = c.b;
}

/*----------------------------------------------------------------------------
 * setcolor
 *--------------------------------------------------------------------------*/
void drawsysfontchar(Bitmap *dest, int xc, int yc, char c, int color, int trans) {
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
        dest->buffer[offset+x] = color;
      /* If not, then check if the transparency flag is set.  If not,
         draw the space in index 0.  Otherwise, the non-pixel data
         won't be drawn, leaving the underlying image alone */
      else if (!trans)
          dest->buffer[offset+x] = 0;
        
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
void drawsysfontstring(Bitmap *dest, int x, int y, char *text, int color, int trans) {
  int i;
  
  for(i = 0; text[i] != 0; i++)
    drawsysfontchar(dest, x+(i*8), y, text[i], color, trans);
}

/*----------------------------------------------------------------------------
 * clear
 *--------------------------------------------------------------------------*/
void clear(Bitmap *dest, int color) {
  memset(dest->buffer, color, SCREEN_WIDTH * SCREEN_HEIGHT);
}

/*----------------------------------------------------------------------------
 * hline
 *--------------------------------------------------------------------------*/
void hline(Bitmap *dest, int x1, int x2, int y, int color) {
  memset(dest->buffer + (y * SCREEN_WIDTH + x1), color, x2-x1+1);
}

/*----------------------------------------------------------------------------
 * vline
 *--------------------------------------------------------------------------*/
void vline(Bitmap *dest, int x, int y1, int y2, int color) {
  int i;
  int pos = y1 * SCREEN_WIDTH + x;
  for (i=0; i < y2-y1+1; i++) {
    dest->buffer[pos] = color;
    pos += SCREEN_WIDTH;
  }
}

/*----------------------------------------------------------------------------
 * createbitmap
 *--------------------------------------------------------------------------*/
Bitmap *createbitmap(int w, int h, Palette *p) {
  Bitmap *b;
  if((b->buffer = (char *)malloc(w*h*sizeof(char))) == NULL) 
    return NULL;
  
  b->w = w;
  b->h = h;
  
  if(p == NULL) {
    b->palette = NULL;
  } else {
    b->palette = (char *)malloc(PALETTE_SIZE * sizeof(char));
    memcpy(b->palette, p, PALETTE_SIZE*sizeof(char));
  }
  
  return b;
}

/*----------------------------------------------------------------------------
 * freebitmap
 *--------------------------------------------------------------------------*/
void freebitmap(Bitmap *b) {
  if (b->buffer)
    free(b->buffer);
  if (b->palette)
    free(b->palette);
  if(b)
    free(b);
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
   
  /* Create the screen object and point it to the VGA memory and a copy of the palette.
  /* Since mode 13 is the only supported mode right now, just set the width and height
   * here.*/
  screen = (Bitmap *)malloc(sizeof(Bitmap));
  screen->w = SCREEN_WIDTH;
  screen->h = SCREEN_HEIGHT;  
  screen->buffer = (char *)0xa0000 + __djgpp_conventional_base;
  screen->palette = (char *)malloc(PALETTE_SIZE * sizeof(char));  

  /* Point sysfont to the VGA font data */
  sysfont = (char *)0xFFA6E + __djgpp_conventional_base;
  
}

/*----------------------------------------------------------------------------
 * graphshutdown
 *--------------------------------------------------------------------------*/
int graphshutdown(void) {
  
  if(screen->palette)
    free(screen->palette);
  
  if(screen)
    free(screen);
  
  __djgpp_nearptr_disable();
}