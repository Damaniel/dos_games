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
#include "imageops.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

Bitmap *loadpcx(char *filename, int loadpalette) {
  FILE *fp;
  PCXheader header;  
  Bitmap *image;
  int buffersize;
  int currentbyte;
  int i;
  unsigned char runsize;
  unsigned char tempbyte;
  
  if ((fp = fopen(filename, "rb")) == NULL)
    return NULL;
  
  /* Read in the header.  Bail if the manufacturer byte isn't set to 10,
     or if the encoding isn't set to RLE. */
  header.manufacturer = fgetc(fp);
  if(header.manufacturer != 10) {
    fclose(fp);
    return NULL;
  }
  header.version = fgetc(fp);
  header.encoding = fgetc(fp);
  if(header.encoding != 1) {
    fclose(fp);
    return NULL;
  }
  
  header.bitsperpixel = fgetc(fp);
  fread(&(header.xmin), sizeof(short), 1, fp);
  fread(&(header.ymin), sizeof(short), 1, fp);
  fread(&(header.xmax), sizeof(short), 1, fp);
  fread(&(header.ymax), sizeof(short), 1, fp);
  fread(&(header.hdpi), sizeof(short), 1, fp);
  fread(&(header.vdpi), sizeof(short), 1, fp);
  fread(header.colormap, sizeof(char), 48, fp);
  header.reserved = fgetc(fp);
  header.nplanes = fgetc(fp);
  fread(&(header.bytesperline), sizeof(short), 1, fp);
  fread(&(header.paletteinfo), sizeof(short), 1, fp);  
  fread(&(header.hscreensize), sizeof(short), 1, fp);
  fread(&(header.vscreensize), sizeof(short), 1, fp);
  fread(header.filler, sizeof(char), 54, fp);
  
  image = (Bitmap *)malloc(sizeof(Bitmap));
  
  image->w = (header.xmax - header.xmin + 1);
  image->h = (header.ymax - header.ymin + 1);
  
  /* Allocate sufficient memory for the buffer (image height * bytes per pixel) */  
  buffersize = (header.bytesperline * (header.ymax - header.ymin + 1)) * sizeof(char);
  image->buffer = (char *)malloc(buffersize);
  
  currentbyte = 0;
  while(currentbyte < buffersize) {
    tempbyte = fgetc(fp);
    if (tempbyte < 192) {
      image->buffer[currentbyte] = tempbyte;
      currentbyte++;
    }
    else {
      runsize = tempbyte - 192;
      tempbyte = fgetc(fp);
      memset(&(image->buffer[currentbyte]), tempbyte, runsize);
      currentbyte+=runsize;
    }
  }
  
  /* Finally, read in the palette data */
  /* First, drop the palette tag byte (should be decimal 12 though) */ 
  tempbyte = fgetc(fp);
    
  /* Then read the data, one byte a a time.
     Note that values in the file range from 0-255, but the VGA expects
     values from 0-63, so each value is divided by 4. 
   */
  
  if (loadpalette) {
    image->palette = (char *)malloc(PALETTE_SIZE * sizeof(char));
    for(i = 0; i < 768; i++) {
      tempbyte = fgetc(fp);
      image->palette[i] = tempbyte >> 2;
    }
  } else {
    image->palette = NULL;
  }
  
  fclose(fp);
  return image;
}

void printpcxheader(PCXheader header) {
  printf("Manufacturer: ");
  switch (header.manufacturer) {
    case 10:
      printf("ZSoft\n");
      break;
    default:
      printf("Unknown!\n");
      break;
  }
  
  printf("Version: ");
  switch (header.version) {
    case 0:
      printf("PC Paintbrush version 2.5\n");
      break;
    case 2:
      printf("PC Paintbrush version 2.8 with palette\n");
      break;
    case 3:
      printf("PC Paintbrush version 2.8 without palette\n");
      break;
    case 4:
      printf("PC Paintbrush for Windows\n");
      break;
    case 5:
      printf("PC Paintbrush 3.0+ or PC Paintbrush for Windows Plus\n");
      break;
    default:
      printf("Unknown\n");
      break;
  }
  
  printf("Encoding: ");
  switch(header.encoding) {
    case 1:
      printf("RLE\n");
      break;
    default:
      printf("Uncompressed or unknown encoding\n");
      break;
  }
  
  printf("Bits per pixel: %d\n", header.bitsperpixel);
  printf("Image width: %d\n", header.xmax - header.xmin + 1);
  printf("Image height: %d\n", header.ymax - header.ymin + 1);
  printf("Horizontal DPI: %d\n", header.hdpi);
  printf("Vertical DPI: %d\n", header.vdpi);
  printf("Number of color planes: %d\n", header.nplanes);
  printf("Number of bytes per line: %d\n", header.bytesperline);
  printf("Palette info: ");
  switch(header.paletteinfo) {
    case 1:
      printf("Color\n");
      break;
    case 2:
      printf("Grayscale\n");
      break;
    default:
      printf("Unknown\n");
      break;
  }
  
  printf("Buffer size required: %d bytes\n", (header.bytesperline * (header.ymax - header.ymin + 1)));
}

