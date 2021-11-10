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
#include "../include/dampbn.h"
#include "../include/palette.h"
#include "../include/uiconsts.h"
#include "../include/render.h"
#include "../include/util.h"

char *g_categories[NUM_CATEGORIES] = {
    "Uncategorized",
    "Miscellaneous",
    "Landscape",
    "Animals",
    "Fantasy",
    "Abstract",
    "Pattern"
};

/*=============================================================================
 * load_picture_file
 *============================================================================*/
Picture *load_picture_file(char *filename) {
  FILE *fp;
  Picture *pic;
  unsigned char magic[2];
  unsigned char compression;
  int i, bytes_processed;
  RGB pic_pal[64];
  unsigned char dummy, first_byte, run_length;

  fp = fopen(filename, "rb");
    if (fp == NULL)
      return NULL;

  /* Check for magic bytes */
  fscanf(fp, "%c%c", &magic[0], &magic[1]);
  if(magic[0] != 'D' || magic[1] != 'P') {
    fclose(fp);
    return NULL;
  }

  /* Set up the Picture object */
  pic = (Picture *)malloc(sizeof(Picture));

  /* Read in the header */
  fread(&(pic->w), 1, sizeof(short), fp);
  fread(&(pic->h), 1, sizeof(short), fp);
  fread(&(pic->category), 1, sizeof(unsigned char), fp);
  fread(pic->image_name, 32, sizeof(char), fp);
  fread(&(pic->num_colors), 1, sizeof(unsigned char), fp);
  fread(&compression, 1, sizeof(unsigned char), fp);
  for(i=0; i<64; i++) {
    pic_pal[i].r = fgetc(fp);
    pic_pal[i].g = fgetc(fp);
    pic_pal[i].b = fgetc(fp);
  }
  for(i=0;i<23;i++) {
    dummy = fgetc(fp);
  }

  /* Set the image portion of the global palette */
  for (i=0; i<64; i++) {
    game_pal[i].r = pic_pal[i].r;
    game_pal[i].g = pic_pal[i].g;
    game_pal[i].b = pic_pal[i].b;
  }

  /* Create required Picture arrays */
  pic->pic_squares = (ColorSquare *)malloc(pic->w * pic->h *
                                           sizeof(ColorSquare));
  pic->draw_order = (OrderItem *)malloc(pic->w * pic->h *
                                        sizeof(OrderItem));

  /* Check compression type and perform appropriate decompression */
  if(compression == COMPRESSION_NONE) {
    for(i=0; i< (pic->w*pic->h); i++) {
      /* Using '+ 1'  since palettes in the Picture go from 1-64, not 0-63 */
      (pic->pic_squares[i]).pal_entry = fgetc(fp) + 1;
      (pic->pic_squares[i]).fill_value = 0;
      (pic->pic_squares[i]).order = -1;
    }
  } else {
    bytes_processed = 0;
    while (bytes_processed < (pic->w * pic->h)) {
      first_byte = fgetc(fp);
      if(first_byte & 0x80) {
        /* found a run.  Load the next byte and write the appropriate
           number of copies to the buffer */
        run_length = fgetc(fp);
        for (i=0;i<run_length;i++) {
          (pic->pic_squares[bytes_processed]).pal_entry =(first_byte & 0x7F)+1;
          (pic->pic_squares[bytes_processed]).fill_value = 0;
          (pic->pic_squares[bytes_processed]).order = -1;
          bytes_processed++;
        }
      } else {
        /* Found a single value */
        (pic->pic_squares[bytes_processed]).pal_entry = first_byte + 1;
        (pic->pic_squares[bytes_processed]).fill_value = 0;
        (pic->pic_squares[i]).order = -1;
        bytes_processed++;
      }
    } 
  }

  fclose(fp);
  return pic;

}

/*=============================================================================
 * free_picture_file
 *============================================================================*/
void free_picture_file(Picture *p) {
  if(p->pic_squares != NULL) 
    free(p->pic_squares);
  if(p->draw_order != NULL)
    free(p->draw_order);
  if(p != NULL)
    free(p);
}


/*=============================================================================
 * load_graphics
 *============================================================================*/
int load_graphics(void) {
  PALETTE res_pal;
  int result;

  result = 0;

  g_numbers  = load_pcx("RES/NUMBERS.PCX", res_pal);
  if(g_numbers == NULL) {
    result = - 1;
  }
  g_bg_lower = load_pcx("RES/BG_LOWER.PCX", res_pal);
  if(g_bg_lower == NULL) {
    result = -1;
  }
  g_bg_right = load_pcx("RES/BG_RIGHT.PCX", res_pal);
  if(g_bg_right == NULL) {
    result = -1;
  }
  g_mainarea = load_pcx("RES/MAINAREA.PCX", res_pal);
  if(g_mainarea == NULL) {
    result = -1;
  }
  g_pal_col  = load_pcx("RES/PAL_COL.PCX", res_pal);
  if(g_pal_col == NULL) {
    result = -1;
  }
  g_draw_cursor = load_pcx("RES/DRAWCURS.PCX", res_pal);
  if(g_draw_cursor == NULL) {
    result = -1;
  }
  g_small_pal = load_pcx("RES/SM_PAL.PCX", res_pal);
  if(g_small_pal == NULL) {
    result = -1;
  }
  g_large_pal = load_pcx("RES/LG_PAL.PCX", res_pal);
  if(g_large_pal == NULL) {
    result = -1;
  }
  g_pal_cursor = load_pcx("RES/PALCURS.PCX", res_pal);
  if(g_pal_cursor == NULL) {
    result = -1;
  }  
  g_wrong = load_pcx("RES/WRONG.PCX", res_pal);
  if(g_wrong == NULL) {
    result = -1;
  }
  g_page_buttons = load_pcx("RES/PAGEBUTN.PCX", res_pal);
  if(g_page_buttons == NULL ) {
    result = -1;
  }
  g_prop_font = load_pcx("RES/PROPFONT.PCX", res_pal);
  if(g_prop_font == NULL ) {
      result = -1;
  }
  return result;
}

/*=============================================================================
 * destroy_graphics
 *============================================================================*/
void destroy_graphics(void) {
  destroy_bitmap(g_numbers);
  destroy_bitmap(g_bg_lower);
  destroy_bitmap(g_bg_right);
  destroy_bitmap(g_mainarea);
  destroy_bitmap(g_pal_col);
  destroy_bitmap(g_draw_cursor);
  destroy_bitmap(g_small_pal);
  destroy_bitmap(g_large_pal);
  destroy_bitmap(g_pal_cursor);
  destroy_bitmap(g_wrong);
  destroy_bitmap(g_page_buttons);
  destroy_bitmap(g_prop_font);
}

/*=============================================================================
 * int_handler
 *============================================================================*/
void int_handler(void) {
  /* do animation stuff here */
  g_int_counter++;
  if(g_int_counter >= FRAME_RATE) {
    g_elapsed_time++;
    g_int_counter=0;
    g_components.render_status_text = 1;
    g_update_screen = 1;
  }
}
END_OF_FUNCTION(int_handler);

/*=============================================================================
 * init_defaults
 *============================================================================*/
void init_defaults(void) {
  int i;
  
  g_draw_cursor_x = 0;
  g_draw_cursor_y = 0;
  g_pic_render_x = 0;
  g_pic_render_y = 0;
  g_draw_position_x = 0;
  g_draw_position_y = 0;
  g_game_done = 0;
  g_palette_page = 0;
  g_cur_color = 1;
  g_prev_color = 1;
  g_elapsed_time = 0;
  g_int_counter = 0;
  g_update_screen = 1;

  for(i=0; i<128; i++)
    g_keypress_lockout[i] = 0;

  /* Variables used in the interrupt handler */
  LOCK_VARIABLE(g_elapsed_time);
  LOCK_VARIABLE(g_int_counter);
  LOCK_VARIABLE(g_components);
  LOCK_VARIABLE(g_update_screen);
  LOCK_FUNCTION(int_handler);

  g_state = STATE_GAME;
}
