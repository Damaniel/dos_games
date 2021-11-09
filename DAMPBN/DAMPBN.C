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
#include "dampbn.h"
#include "palette.h"
#include "uiconsts.h"

/* Some stuff to cut down the executable size */
BEGIN_GFX_DRIVER_LIST
   GFX_DRIVER_VGA
END_GFX_DRIVER_LIST

BEGIN_COLOR_DEPTH_LIST
   COLOR_DEPTH_8
END_COLOR_DEPTH_LIST

BEGIN_JOYSTICK_DRIVER_LIST
END_JOYSTICK_DRIVER_LIST

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
 * clear_render_components
 *============================================================================*/
void clear_render_components(RenderComponents *c) {
  c->render_main_area_squares = 0;
  c->render_palette_area = 0;
  c->render_palette = 0;
  c->render_ui_components = 0;
  c->render_overview_display = 0;
  c->render_status_text = 0;
  c->render_draw_cursor = 0;
  c->render_all = 0;
}

/*=============================================================================
 * render_main_area_squares
 *============================================================================*/
void render_main_area_squares(BITMAP *dest, int x_off, int y_off) {
  int i,j;
  int pal_offset;
  ColorSquare c;

  /* Iterate through each square of the visible area of the picture and
     draw either the related color number or color swatch for that square */
  for (j = y_off; j < y_off + PLAY_AREA_H; j++) {
    for (i = x_off; i < x_off + PLAY_AREA_W; i++) {
      c = g_picture->pic_squares[j * g_picture->w + i];
      pal_offset = c.pal_entry;
      blit(g_numbers, 
           dest, 
           pal_offset * NUMBER_BOX_WIDTH,
           0,
           DRAW_AREA_X + ((i-x_off) * NUMBER_BOX_RENDER_X_OFFSET),
           DRAW_AREA_Y + ((j-y_off) * NUMBER_BOX_RENDER_Y_OFFSET),
           NUMBER_BOX_WIDTH, 
           NUMBER_BOX_HEIGHT);
    }
  }
}

/*=============================================================================
 * render_screen
 *============================================================================*/
void render_screen(BITMAP *dest, RenderComponents c) {
  int start_index, palette_color, square_x, square_y;
  int swatch_x, swatch_y;
  int i, j;

  /* Draw the static UI components */
  if (c.render_ui_components || c.render_all) {
    blit(g_bg_right, dest, 0, 0, RIGHT_SIDE_PANEL_X, RIGHT_SIDE_PANEL_Y,
         g_bg_right->w, g_bg_right->h);
    blit(g_bg_lower, dest, 0, 0, BOTTOM_PANEL_X, BOTTOM_PANEL_Y,
         g_bg_lower->w, g_bg_lower->h);
    blit(g_mainarea, dest, 0, 0, MAIN_AREA_X, MAIN_AREA_Y, 
         g_mainarea->w, g_mainarea->h);
  }

  /* Draw the palette columns */
  if (c.render_palette_area || c.render_all) {
    /* Render palette columns */
    blit(g_pal_col, dest, 0, 0, PALETTE_COLUMN_1_X, PALETTE_COLUMN_Y,
         g_pal_col->w, g_pal_col->h);
    blit(g_pal_col, dest, 0, 0, PALETTE_COLUMN_2_X, PALETTE_COLUMN_Y,
         g_pal_col->w, g_pal_col->h);
    blit(g_pal_col, dest, 0, 0, PALETTE_COLUMN_3_X, PALETTE_COLUMN_Y,
         g_pal_col->w, g_pal_col->h);
    blit(g_pal_col, dest, 0, 0, PALETTE_COLUMN_4_X, PALETTE_COLUMN_Y,
         g_pal_col->w, g_pal_col->h);

    /* Draw the palette numbers depending on the page.  If the box falls outside
       of the number of valid colors, draw a gray box instead */
    if (g_palette_page == 0)
      start_index = PALETTE_PAGE_1_START;
    else
      start_index = PALETTE_PAGE_2_START;

    for (j=0; j<NUM_PALETTE_COLUMNS; j++) {
      for(i=0; i<NUM_PALETTE_ROWS; i++) {
        palette_color = start_index + (j * NUM_PALETTE_ROWS) + i + 1;
        square_x = PALETTE_AREA_X + (j * PALETTE_COLUMN_WIDTH);
        square_y = PALETTE_AREA_Y + (i * PALETTE_ITEM_HEIGHT);
        if (palette_color > g_picture->num_colors) {
          blit(g_numbers, dest, 0, 0, square_x, square_y,
               NUMBER_BOX_WIDTH, NUMBER_BOX_HEIGHT);
        } else {
          blit(g_numbers, dest, palette_color * NUMBER_BOX_WIDTH, 0,
               square_x, square_y, NUMBER_BOX_WIDTH, NUMBER_BOX_HEIGHT);
        }
      }
    }
  }

  /* Draw the palette color swatches */
  if (c.render_palette || c.render_all) {
    if (g_palette_page == 0)
      start_index = PALETTE_PAGE_1_START;
    else
      start_index = PALETTE_PAGE_2_START;

    for (j=0; j<NUM_PALETTE_COLUMNS; j++) {
      for (i=0; i<NUM_PALETTE_ROWS; i++) {
        palette_color = start_index + (j * NUM_PALETTE_ROWS ) + i + 1;
        swatch_x = SWATCH_AREA_X + (j * PALETTE_COLUMN_WIDTH);
        swatch_y = SWATCH_AREA_Y + (i * PALETTE_ITEM_HEIGHT);
        if (palette_color > g_picture->num_colors) {
          blit(g_small_pal, dest, 0, 0, swatch_x, swatch_y,
               NUMBER_BOX_WIDTH, NUMBER_BOX_HEIGHT);
        } else {
          blit(g_small_pal, dest, palette_color * PALETTE_BOX_WIDTH, 0,
               swatch_x, swatch_y, PALETTE_BOX_WIDTH, PALETTE_BOX_HEIGHT);
        }
      }
    }
  }

  /* Draw the overview window */
  if (c.render_overview_display || c.render_all) {
  }

  /* Draw updated status text in the lower left part of the display */
  if (c.render_status_text || c.render_all) {
  }

  /* Draw the squares in the play area */
  if(c.render_main_area_squares || c.render_all) {
    render_main_area_squares(dest, g_pic_render_x, g_pic_render_y);
  }

  /* Draw the various cursors */
  if(c.render_draw_cursor || c.render_all) {
    draw_sprite(dest, g_draw_cursor,
                DRAW_AREA_X + DRAW_CURSOR_WIDTH * g_draw_cursor_x, 
                DRAW_AREA_Y + DRAW_CURSOR_WIDTH * g_draw_cursor_y);
  }

}

/*=============================================================================
 * load_palette_swatches
 *============================================================================*/
void load_palette_swatches(void) {
  int i, x, y;

  for(i=0; i<MAX_COLORS; i++) {
     x = ( (i + 1) * PALETTE_BOX_WIDTH) + 1;
     rectfill(g_small_pal, x, 1,
              x+(PALETTE_BOX_INTERIOR_WIDTH-1),
              PALETTE_BOX_INTERIOR_HEIGHT, i);
     x = ( (i + 1) * PALETTE_BOX_HEIGHT) + 1;
     rectfill(g_large_pal, x, 1, 
              x+(NUMBER_BOX_INTERIOR_WIDTH-1),
              NUMBER_BOX_INTERIOR_HEIGHT, i);
  }

}

/*=============================================================================
 * load_graphics
 *============================================================================*/
int load_graphics(void) {
  PALETTE res_pal;
  int result;

  result = 0;

  g_numbers  = load_pcx("res/numbers.pcx", res_pal);
  if(g_numbers == NULL) {
    result = - 1;
  }
  g_bg_lower = load_pcx("res/bg_lower.pcx", res_pal);
  if(g_bg_lower == NULL) {
    result = -1;
  }
  g_bg_right = load_pcx("res/bg_right.pcx", res_pal);
  if(g_bg_right == NULL) {
    result = -1;
  }
  g_mainarea = load_pcx("res/mainarea.pcx", res_pal);
  if(g_mainarea == NULL) {
    result = -1;
  }
  g_pal_col  = load_pcx("res/pal_col.pcx", res_pal);
  if(g_pal_col == NULL) {
    result = -1;
  }
  g_draw_cursor = load_pcx("res/drawcurs.pcx", res_pal);
  if(g_draw_cursor == NULL) {
    result = -1;
  }
  g_small_pal = load_pcx("res/sm_pal.pcx", res_pal);
  if(g_small_pal == NULL) {
    result = -1;
  }
  g_large_pal = load_pcx("res/lg_pal.pcx", res_pal);
  if(g_large_pal == NULL) {
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
}

/*=============================================================================
 * int_handler
 *============================================================================*/
void int_handler(void) {
  /* do animation stuff here */
}

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

  for(i=0; i<128; i++)
    g_keypress_lockout[i] = 0;
}

/*=============================================================================
 * main
 *============================================================================*/
int main(void) {
  int status, i;
  int done, update_components;

  allegro_init();
  install_keyboard();
  install_timer();

  install_int(int_handler, 1000/FRAME_RATE);

  set_gfx_mode(GFX_VGA, 320, 200, 0, 0);

  status = load_graphics();
  if (status != 0) {
    set_gfx_mode(GFX_TEXT, 80, 25, 0, 0);
    printf("Unable to load all graphics!\n");
    allegro_exit();
    exit(1);
  }
  g_picture = load_picture_file("TOOLS/IMG1.PIC");

  set_palette(game_pal);
  load_palette_swatches();

  init_defaults();

  clear_render_components(&g_components);
  g_components.render_all = 1;

  render_screen(screen, g_components);

  while(!g_game_done) {  
    update_components = process_input(0);
    if (update_components)
      render_screen(screen, g_components);
  }

  free_picture_file(g_picture);
  destroy_graphics();
  set_gfx_mode(GFX_TEXT, 80, 25, 0, 0);
  allegro_exit();

  return 0;
}
