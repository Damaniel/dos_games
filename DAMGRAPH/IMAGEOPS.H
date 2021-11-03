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
#ifndef __IMAGEOPS_H__
#define __IMAGEOPS_H__
#include "damgraph.h"

/* 
 Info provided by ZSoft documentation about the PCX format:

  Byte      Item          Size   Description/Comments 
  0         Manufacturer 1      Constant Flag, 10 = ZSoft .pcx 
  1         Version      1      Version information 
                                0 = Version 2.5 of PC Paintbrush 
                                2 = Version 2.8 w/palette information 
                                3 = Version 2.8 w/o palette information 
                                4 = PC Paintbrush for Windows(Plus for
                                    Windows uses Ver 5) 
                                5 = Version 3.0 and > of PC Paintbrush
                                    and PC Paintbrush +, includes
                                    Publisher's Paintbrush . Includes
                                    24-bit .PCX files 
  2         Encoding      1     1 = .PCX run length encoding 
  3         BitsPerPixel  1     Number of bits to represent a pixel
                                    (per Plane) - 1, 2, 4, or 8 
  4         Window        8     Image Dimensions: Xmin,Ymin,Xmax,Ymax 
  12         HDpi          2     Horizontal Resolution of image in DPI* 
  14         VDpi          2     Vertical Resolution of image in DPI* 
  16         Colormap     48     Color palette setting, see text 
  64         Reserved      1     Should be set to 0. 
  65         NPlanes       1     Number of color planes 
  66         BytesPerLine  2     Number of bytes to allocate for a scanline
                                    plane.  MUST be an EVEN number.  Do NOT
                                    calculate from Xmax-Xmin. 
  68         PaletteInfo   2     How to interpret palette- 1 = Color/BW,
                                    2 = Grayscale (ignored in PB IV/ IV +) 
  70         HscreenSize   2     Horizontal screen size in pixels. New field
                                    found only in PB IV/IV Plus 
  72         VscreenSize   2     Vertical screen size in pixels. New field
                                    found only in PB IV/IV Plus 
  74         Filler       54     Blank to fill out 128 byte header.  Set all
                                    bytes to 0 
*/
typedef struct {
  unsigned char  manufacturer;
  unsigned char  version;
  unsigned char  encoding;
  unsigned char  bitsperpixel;
  unsigned short xmin;
  unsigned short ymin;
  unsigned short xmax;
  unsigned short ymax;
  unsigned short hdpi;
  unsigned short vdpi;
  unsigned char  colormap[48];
  unsigned char  reserved;
  unsigned char  nplanes;
  unsigned short bytesperline;
  unsigned short paletteinfo;
  unsigned short hscreensize;
  unsigned short vscreensize;
  unsigned char  filler[54];
} PCXheader;

void printpcxheader(PCXheader header);
Bitmap *loadpcx(char *filename, int loadpalette);

#endif
