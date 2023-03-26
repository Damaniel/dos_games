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
 * drawsysfontchar
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
 * drawsysfontstring
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
 * blitsprite
 *--------------------------------------------------------------------------*/
void blitsprite(Bitmap *dest, Sprite *src, int frame, int x, int y) {
  int xoff;
  int frameoff;
  
  if (frame <0) {
    frameoff = src->curframe;
  } else
    frameoff = (frame < src->numframes) ? frame : 0;
  
  xoff = src->width * frameoff;

  clippedblit(dest, src->spritesheet, xoff, 0, x, y, src->width, src->height,
              src->transidx);
}

/*----------------------------------------------------------------------------
 * blitbitmap
 *--------------------------------------------------------------------------*/
void blitbitmap(Bitmap *dest, Bitmap *src, int x, int y, int transidx) {
  clippedblit(dest, src, 0, 0, x, y, src->w, src->h, transidx);
}

/*----------------------------------------------------------------------------
 * clippedblit
 *--------------------------------------------------------------------------*/
void clippedblit(Bitmap *dest, Bitmap *src, int srcx, int srcy, 
                 int destx, int desty, int w, int h, int transidx) {
  
  int c_sx, c_sy, c_dx, c_dy, c_w, c_h;
  
  /* Source blit region needs to be fully within the bounds of
     the source bitmap */   
  if(srcx < 0 || srcy < 0)
  {
    return;
  }
  if(srcx + w > src->w || srcy + h > src->h) {
    return;
  }
  /* If the entire source bitmap will lie outside of the destination
     bitmap, just don't draw it */
  if (destx + w < 0 || desty + h < 0) {
    return;
  }
  if (destx >= SCREEN_WIDTH || desty >= SCREEN_HEIGHT) {
    return;
  }
  
  c_sx = srcx;
  c_sy = srcy;
  c_dx = destx;
  c_dy = desty;
  c_w = w;
  c_h = h;
  
  /* Clip the left edge */
  if (destx < 0) {
    c_dx = 0;
    c_sx += -destx;
    c_w -= -destx;
  }
  
  /* Clip the top edge */
  if (desty < 0) {
    c_dy = 0;
    c_sy += -desty;
    c_h -= -desty;
  }
 
  /* Clip the right edge */
  if (destx + w > dest->w)
    c_w = (dest->w - destx);
    
  /* Clip the bottom edge */
  if (desty + h > dest->h)
    c_h = (dest->h - desty);
  
  /* If the resulting image width or height is <= 0, don't blit.
     Note that the previous checks should have prevented this condition, but 
     it doesn't hurt to do the extra check */
  if (c_w <=0 || c_h <=0 ) {
    //printf("Not blitting, resulting clipped width or height is <= 0\n");
    return;
  } 
  
  if (transidx < 0)
    blit(dest, src, c_sx, c_sy, c_dx, c_dy, c_w, c_h);
  else
    transblit(dest, src, c_sx, c_sy, c_dx, c_dy, c_w, c_h, transidx);
}

/*----------------------------------------------------------------------------
 * blit
 *--------------------------------------------------------------------------*/      
void blit(Bitmap *dest, Bitmap *src, int srcx, int srcy, int destx, 
          int desty, int w, int h) {
 
  /* This does no bounds checking.  If you want to ensure all blits
     have in-bounds sources and destinations, use clippedblit(), 
     which does any required clipping.
     
     This version is plenty fast for my needs - rough estimates show that 
     if doing nothing else, it can update the entire screen at approximately
     120 fps on a low end 386, and more like 1700 fps on a mid spec first 
     gen Pentium. 
  */
   int i;

   for(i=0; i<h; i++) {
     memcpy(&dest->buffer[(desty+i) * dest->w + destx],
            &src->buffer[(srcy+i) * src->w + srcx], 
            w*sizeof(char));
   }
   
   /*
     For historical purposes, here's some assembly I wrote to do the
     same thing.  Since it uses movsb instead of using a word based
     approach with exceptions for odd line lengths, it ended up about 50%
     slower than the memcpy version.  Nobody said my assembly was any
     good.
     
     asm(
       "movl %0, %%eax\n\t"
       "movl %1, %%edi\n\t"
       "movl %2, %%esi\n\t"             
       "%=:\n\t"
       "movl %3, %%ecx\n\t"        
       "rep movsb\n\t"
       "add %4, %%esi\n\t"       
       "add %5, %%edi\n\t"       
       "dec %%eax\n\t"
       "jnz %=b\n\t"
       :
       : "m"(h), 
         "m"(&dest->buffer[desty * SCREEN_WIDTH + destx]),
         "m"(&src->buffer[srcy * src->w + srcx]), 
         "m"(w),
         "m"(src->w - w),
         "m"(SCREEN_WIDTH-w)
       : "%esi", "%edi", "%eax", "%ecx");
    */   
}

/*----------------------------------------------------------------------------
 * transblit
 *--------------------------------------------------------------------------*/  
void transblit(Bitmap *dest, Bitmap *src, int srcx, int srcy, int destx, 
          int desty, int w, int h, int transidx) {
         
  /* Essentially the same idea as blit(), but treats the color index 
     <transidx> as transparent, not drawing the pixel in those places.
     Since this is done with dual loops instead of memcpy, it's a fair
     bit slower and should be reserved for bitmaps that need it */
  int i, j;
  int sidx, didx;
   
  sidx = srcy * src->w + srcx;
  didx = desty * dest->w + destx;
  
  for(j = 0; j < h; j++) {
    for(i = 0; i < w; i++) {
      if (src->buffer[sidx] != transidx)
        dest->buffer[didx] = src->buffer[sidx];
      sidx++;
      didx++;
     }
    sidx += src->w - w;
    didx += dest->w - w;
  }
}

/*----------------------------------------------------------------------------
 * createbitmap
 *--------------------------------------------------------------------------*/
Bitmap *createbitmap(int w, int h, Palette *p) {
  Bitmap *b;
  b = (Bitmap *)malloc(sizeof(Bitmap));
  
  if((b->buffer = (char *)malloc(w*h*sizeof(char))) == NULL) 
    return NULL;
  
  b->w = w;
  b->h = h;
  
  memset(b->buffer, 0, w*h*sizeof(char));
  
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
  if (b->buffer != NULL) {
    free(b->buffer);
  }
  if (b->palette != NULL) {
    free(b->palette);  
  }  
  if(b != NULL) {
    free(b);
  }
}

/*----------------------------------------------------------------------------
 * loadsprite
 *--------------------------------------------------------------------------*/
Sprite *loadsprite(char *filename, int width, int height, int numframes,
                   int transidx) {
  Sprite *s;
  
  s = (Sprite *)malloc(sizeof(Sprite));
  
  /* Load the PCX bitmap */
  s->spritesheet = loadpcx(filename, 0);
  
  if (s->spritesheet==NULL) {
   if(s)
     free(s);
   return NULL;
  }
  
  /* Check to see if the spritesheet is big enough for the sprite */
  if(s->spritesheet->w < (width * numframes) ||
     s->spritesheet->h < height) {
        freebitmap(s->spritesheet);
        if(s)
          free(s);
        return NULL;
  }
  
  s->width = width;
  s->height = height;
  s->numframes = numframes;
  s->curframe = 0;
  s->animspeed = 0;
  s->animcounter = 0;
  s->transidx = transidx;
  s->behind = createbitmap(width, height, NULL);
  
  return s;
}

/*----------------------------------------------------------------------------
 * freesprite
 *--------------------------------------------------------------------------*/
void freesprite(Sprite *s) {
  if(s->behind)
    freebitmap(s->behind);
  if(s->spritesheet)
    freebitmap(s->spritesheet);
  if(s)
    free(s);
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