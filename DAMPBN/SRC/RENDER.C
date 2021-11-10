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
#include "../include/dampbn.h"
#include "../include/render.h"
#include "../include/uiconsts.h"
#include "../include/palette.h"

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
  c->render_palette_cursor = 0;
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
 * render_palette_item_at
 *============================================================================*/
void render_palette_item_at(BITMAP *dest, int palette_index, int change_page) {
  int draw_index;
  int draw_x_pos, draw_y_pos;

  if(palette_index >= FIRST_COLOR_ON_SECOND_PAGE) {
    draw_index = palette_index - PALETTE_COLORS_PER_PAGE;
    if (change_page)
      g_palette_page = 1;
  }
  else {
    draw_index = palette_index;
    if (change_page)
      g_palette_page = 0;
  }

  /* Draw the color index box */
  draw_x_pos = PALETTE_AREA_X + (((draw_index - 1) / NUM_PALETTE_ROWS) * 
            PALETTE_COLUMN_WIDTH);
  draw_y_pos = PALETTE_AREA_Y + (((draw_index - 1) % NUM_PALETTE_ROWS) *
            PALETTE_ITEM_HEIGHT);     
  if (palette_index > g_picture->num_colors) {
    blit(g_numbers, dest, 0, 0,
         draw_x_pos, draw_y_pos, NUMBER_BOX_WIDTH, NUMBER_BOX_HEIGHT);
  } else {
    blit(g_numbers, dest, palette_index * NUMBER_BOX_WIDTH, 0,
         draw_x_pos, draw_y_pos, NUMBER_BOX_WIDTH, NUMBER_BOX_HEIGHT);
  }

  /* Draw the color swatch box */
  draw_x_pos = SWATCH_AREA_X + (((draw_index - 1) / NUM_PALETTE_ROWS) *
               PALETTE_COLUMN_WIDTH);
  draw_y_pos = SWATCH_AREA_Y + (((draw_index - 1) % NUM_PALETTE_ROWS) *
               PALETTE_ITEM_HEIGHT);            
  if (palette_index > g_picture->num_colors) {
    blit(g_small_pal, dest, 0, 0,
         draw_x_pos, draw_y_pos, PALETTE_BOX_WIDTH, PALETTE_BOX_HEIGHT);
  } else {
    blit(g_small_pal, dest, palette_index * PALETTE_BOX_WIDTH, 0,
         draw_x_pos, draw_y_pos, PALETTE_BOX_WIDTH, PALETTE_BOX_HEIGHT);
  }

}

/*=============================================================================
 * render_main_area_square_at
 *============================================================================*/
void render_main_area_square_at(BITMAP *dest, int tl_x, int tl_y,
                               int off_x, int off_y) {
  ColorSquare c;
  int pal_offset, color_offset;

  c = g_picture->pic_squares[(tl_y + off_y) * g_picture->w + (tl_x +off_x)];
  pal_offset = c.pal_entry;  
  color_offset = c.fill_value;

  if(color_offset == 0) {
    blit(g_numbers, 
        dest, 
        pal_offset * NUMBER_BOX_WIDTH,
        0, 
        DRAW_AREA_X + (off_x * NUMBER_BOX_RENDER_X_OFFSET),
        DRAW_AREA_Y + (off_y * NUMBER_BOX_RENDER_Y_OFFSET),
        NUMBER_BOX_WIDTH, 
        NUMBER_BOX_HEIGHT);
  } else {
    blit(g_large_pal, 
        dest, 
        color_offset * NUMBER_BOX_WIDTH,
        0, 
        DRAW_AREA_X + (off_x * NUMBER_BOX_RENDER_X_OFFSET),
        DRAW_AREA_Y + (off_y * NUMBER_BOX_RENDER_Y_OFFSET),
        NUMBER_BOX_WIDTH, 
        NUMBER_BOX_HEIGHT);    
    /* Is it marked with the correct color?  If not, draw an X on it */
    if (pal_offset != color_offset) {
      draw_sprite(dest, g_wrong, 
                  DRAW_AREA_X + (off_x * NUMBER_BOX_RENDER_X_OFFSET),
                  DRAW_AREA_Y + (off_y * NUMBER_BOX_RENDER_Y_OFFSET));
    }
  }
}

/*=============================================================================
 * render_screen
 *============================================================================*/
void render_screen(BITMAP *dest, RenderComponents c) {
  int start_index, palette_color, square_x, square_y;
  int swatch_x, swatch_y, pal_index, pal_x, pal_y;
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
    /* Draw the palette numbers depending on the page.  If the box falls outside
       of the number of valid colors, draw a gray box instead */
    if (g_palette_page == 0)
      start_index = FIRST_COLOR_ON_FIRST_PAGE;
    else
      start_index = FIRST_COLOR_ON_SECOND_PAGE;

    for (i=0; i < PALETTE_COLORS_PER_PAGE; i++)
      render_palette_item_at(dest, start_index + i, 0);

    /* Draw the page buttons, with the appropriate pressed / non-pressed
       state for each of them */
    if (g_palette_page == 0) {
      /* Page 1 pressed, page 2 not */
      blit(g_page_buttons, dest, PAGE_BUTTON_WIDTH, 0, 
           PAGE_1_BUTTON_X, PAGE_1_BUTTON_Y, PAGE_BUTTON_WIDTH,
           PAGE_BUTTON_HEIGHT);
      blit(g_page_buttons, dest, 0, PAGE_BUTTON_HEIGHT, 
           PAGE_2_BUTTON_X, PAGE_2_BUTTON_Y, PAGE_BUTTON_WIDTH,
           PAGE_BUTTON_HEIGHT);
    } else {
      /* Page 2 pressed, page 1 not */
      blit(g_page_buttons, dest, 0, 0, 
           PAGE_1_BUTTON_X, PAGE_1_BUTTON_Y, PAGE_BUTTON_WIDTH,
           PAGE_BUTTON_HEIGHT);
      blit(g_page_buttons, dest, PAGE_BUTTON_WIDTH, PAGE_BUTTON_HEIGHT, 
           PAGE_2_BUTTON_X, PAGE_2_BUTTON_Y, PAGE_BUTTON_WIDTH,
           PAGE_BUTTON_HEIGHT);
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
    for(i = 0; i < PLAY_AREA_W; i++) {
      for(j = 0; j < PLAY_AREA_H; j++) {
        render_main_area_square_at(dest, g_pic_render_x, g_pic_render_y, i, j);
      }
    }
  }

  /* Draw the various cursors */
  if(c.render_draw_cursor || c.render_all) {
    /* Redraw the background locations of where the cursor was and now is */
    render_main_area_square_at(dest, g_pic_render_x, g_pic_render_y, 
                               g_old_draw_cursor_x, g_old_draw_cursor_y);
    render_main_area_square_at(dest, g_pic_render_x, g_pic_render_y, 
                               g_draw_cursor_x, g_draw_cursor_y);    
    /* Draw the cursor itself */                          
    draw_sprite(dest, g_draw_cursor,
                DRAW_AREA_X + DRAW_CURSOR_WIDTH * g_draw_cursor_x, 
                DRAW_AREA_Y + DRAW_CURSOR_WIDTH * g_draw_cursor_y);              
  }

  if(c.render_palette_cursor || c.render_all) {
    if (g_palette_page == 1) 
      pal_index = g_cur_color - ( NUM_PALETTE_COLUMNS * NUM_PALETTE_ROWS) - 1;
    else
      pal_index = g_cur_color - 1;

    pal_x = PALETTE_AREA_X + ((pal_index / NUM_PALETTE_ROWS) * 
            PALETTE_COLUMN_WIDTH);
    pal_y = PALETTE_AREA_Y + ((pal_index % NUM_PALETTE_ROWS) *
            PALETTE_ITEM_HEIGHT);

    /* Redraw where the cursor used to be */
    render_palette_item_at(dest, g_prev_color, 0);
    /* Draw the cursor in the new location */
    draw_sprite(dest, g_pal_cursor, pal_x, pal_y);
  }

  if(c.render_debug) {
    textprintf(dest, font, 5, 171, 209, "draw index = %d   ", g_cur_color);    
    textprintf(dest, font, 5, 181, 209, "draw pos = %d, %d     ",
               g_draw_position_x, g_draw_position_y);  
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
