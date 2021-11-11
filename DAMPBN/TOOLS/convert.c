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
#include <allegro.h>
#include <stdio.h>
#include <stdlib.h>
#include "../include/palette.h"

/* Some stuff to cut down the executable size */
BEGIN_GFX_DRIVER_LIST
   GFX_DRIVER_MODEX
END_GFX_DRIVER_LIST

BEGIN_COLOR_DEPTH_LIST
   COLOR_DEPTH_8
END_COLOR_DEPTH_LIST

/*
 * convert - a tool to take PCX images (generally between 20x16 and 320x200
 *           in size, and from 1 to 64 colors) and convert them to the format
 *           used by my paint by number game.  See notes.txt for info about
 *           the file format.
 *           
 * Usage: convert <input_file> <output_file> <category_id> <title>
 *                <palette_size> <compression> 
 *
 * Example: convert image1.pcx image1.pic 0 "Test Image" 64 1
 *
 * Values for compression:
 *  0 - uncompressed
 *  1 - RLE
 *  2 - auto - try both and pick the smallest
 */

/* 
 * get_rle_size()
 *
 * Quick and dirty function to calculate the size of data we'd get if we used
 * RLE encoding.  That way, if we set up auto detection of compression, we 
 * can choose whether to use compressed or uncompressed data.
 *
 * Yes, this code is pretty much copy-pasted from main().
 */
int get_rle_size(BITMAP *b) {
  int cur_val, run_val, run_count, pixel_count, size;

  pixel_count = 0;
  run_count = 0;
  size = 0;

  /* Loop until we've gone through all of the pixels */
  while(pixel_count < b->w * b->h) {
    run_count = 0;
    /* Convert a linear count to an x, y coordinate and get the pixel */
    cur_val = getpixel(b, (pixel_count % b->w), (pixel_count / b->w));
    run_val = cur_val;
    /* The RLE encoder shouldn't save color indices higher than 64 */
    if(run_val >=64) {
      printf("Invalid index for palette!  All colors must be between 0-63!\n");
      printf("Run value detected is %d\n", run_val);
      allegro_exit();
      exit(1);
    }
    /* Loop until the current run ends, we reach the limit for a run size
       (255) or we reach the end of the image data during a run */
    while(run_val == cur_val && run_count < 255 && pixel_count < b->w * b->h) {
      pixel_count++;
      run_count++;
      cur_val = getpixel(b, (pixel_count % b->w), (pixel_count / b->w));
    }
    /* The size of the RLE block increases by 1 for single instances, and 2
     * for runs (one byte for run flag + color, one byte for run length) */
    if(run_count > 1) {
      size+=2;
    } else {
      size++;
    }
  }  

  return size;
}

/*
 * main()
 *
 * Quick. Dirty. Ugly. Yes, this program takes 7 arguments.
 */
int main(int argc, char *argv[]) {

  BITMAP *b;
  PALETTE p;
  FILE *fp;
  char *infile, *outfile, *title;
  int category, pal_size, compression;
  int i, j, size, cur_val, run_val, run_count, pixel_count, do_rle;

  if (argc < 7 ) {
    printf("Usage: convert <input_file> <output_file> <category_id> ");
    printf("<title> <palette_size> <compression>\n");
    printf("  Example: convert image1.pcx image1.pic 0 \"Test Image\" 64 0\n");
    exit(1);
  }

  /* Get the relevant info from the command line */
  infile = argv[1];
  outfile = argv[2];
  title = argv[4];
  category = atoi(argv[3]);
  if (category < 0 || category > 255) {
    printf("Invalid category!  Must be between 0 and 255.\n");
    exit(1);
  }

  pal_size = atoi(argv[5]);
  /* If atoi() fails, bail. */
  if (pal_size < 1 || pal_size > 64) {
    printf("Invalid palette size!  Must be between 1 and 64.\n");
    exit(1);
  }

  compression = atoi(argv[6]);
  if (compression < 0 || compression > 2) {
    printf("Invalid compression!  Legal values are 0, 1, or 2.\n");
    exit(1);
  }

  /* Initialize allegro so we can get image and palette functions */
  allegro_init();

  /* get the input image (and the palette) */
  b = load_bitmap(infile, p);
  if(b == NULL) {
    printf("Unable to open %s!\n", infile);
    printf("%d\n", errno);
    allegro_exit();
    exit(1);
  }

  /* Set the first pal_size palette entries of the system palette to the
     colors of the image */
  for(i=0; i<pal_size; i++) {
    game_pal[i].r = p[i].r;
    game_pal[i].g = p[i].g;
    game_pal[i].b = p[i].b;
  }

  /* Figure out whether we're doing compression or not */
  if (compression == 0) {
    printf("Creating uncompressed file...\n");
    do_rle = 0;
  } else if (compression == 1) {
    printf("Creating RLE encoded file...\n");
    do_rle = 1;
  } else {
    printf("Autodetecting optimal compression...\n");
    size = get_rle_size(b);
    if(size < b->w * b->h) {
      printf("%d < %d, doing RLE\n", size, (b->w * b->h));
      do_rle = 1;
    } else {
      printf("%d > %d, doing uncompressed\n", size, (b->w * b->h));
      do_rle = 0;
    }
  }

  /* Open and start writing the file */
  fp = fopen(outfile, "wb");
  if (fp == NULL) {
    printf("Unable to open output file %s!\n", outfile);
    allegro_exit();
    exit(1);
  }

  /* Write the header */
  fprintf(fp, "DP");
  fwrite(&(b->w), sizeof(unsigned short), 1, fp);
  fwrite(&(b->h), sizeof(unsigned short), 1, fp);
  fwrite(&category, sizeof(unsigned char), 1, fp);

  if(strlen(title) >= 32) {
    fwrite(title, sizeof(unsigned char), 32, fp);
  } else {
    fwrite(title, sizeof(unsigned char), strlen(title), fp);
    for(i=0;i<32-strlen(title);i++)
      fputc(0x00, fp);
  }

  fwrite(&pal_size, sizeof(unsigned char), 1, fp);
  fwrite(&do_rle, sizeof(unsigned char), 1, fp);
  for(i=0; i<64; i++) {
    fwrite(&(game_pal[i].r), sizeof(unsigned char), 1, fp);
    fwrite(&(game_pal[i].g), sizeof(unsigned char), 1, fp);
    fwrite(&(game_pal[i].b), sizeof(unsigned char), 1, fp);
  }
  for(i=0;i<23;i++)
    fputc(0x00, fp);

  if (do_rle == 0) {
    /* No compression */
    for(j=0; j<b->h; j++) {
      for(i=0; i<b->w; i++) {
        fputc(getpixel(b, i, j), fp);
      }
    }
  }
  else {
    /* Run length encoding */
    pixel_count = 0;
    run_count = 0;
    while(pixel_count < b->w * b->h) {
      run_count = 0;
      cur_val = getpixel(b, (pixel_count % b->w), (pixel_count / b->w));
      run_val = cur_val;
      if(run_val >=64) {
        printf("Invalid index for palette!  All colors must be ");
        printf("between 0-63!\n");
        fclose(fp);
        allegro_exit();
        exit(1);
      }
      while(run_val == cur_val && run_count < 255 && 
            pixel_count < b->w * b->h) {
        pixel_count++;
        run_count++;        
        cur_val = getpixel(b, (pixel_count % b->w), (pixel_count / b->w));
      }
      if(run_count > 1) {
        fputc(0x80 | run_val, fp);
        fputc(run_count, fp);
      } else {
        fputc(run_val, fp);
      }
    }
  }

  fclose(fp);

  printf("Wrote %s\n", outfile);
  allegro_exit();

  return 0;

}
