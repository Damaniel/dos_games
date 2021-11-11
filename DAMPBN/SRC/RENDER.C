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
#include "../include/globals.h"

/* Some stuff to cut down the executable size */
BEGIN_GFX_DRIVER_LIST
   GFX_DRIVER_VGA
END_GFX_DRIVER_LIST

BEGIN_COLOR_DEPTH_LIST
   COLOR_DEPTH_8
END_COLOR_DEPTH_LIST

BEGIN_JOYSTICK_DRIVER_LIST
END_JOYSTICK_DRIVER_LIST

#define FONT_ENTRIES    96

/* Width and height of all characters from ASCII values 32 to 127 in the
   proportional font*/

int g_prop_font_width[FONT_ENTRIES] = {
  3, 1, 3, 5, 5, 5, 5, 1, 2, 2, 3, 5, 2, 5, 1, 3,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 1, 2, 3, 3, 3, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 2, 3, 2, 3, 5,
  2, 5, 5, 4, 5, 5, 4, 5, 4, 2, 3, 4, 2, 5, 5, 5,
  5, 5, 4, 5, 4, 5, 5, 5, 5, 5, 5, 3, 1, 3, 5, 5
};

int g_prop_font_height = 7;

int g_prop_font_offset[FONT_ENTRIES] = {
  0,     4,   5,   8,  13,  18,  23,  28,  29,  31,  33,  36,  41,  43,  48,  49, 
   52,  57,  62,  67,  72,  77,  82,  87,  92,  97, 102, 103, 105, 108, 111, 114, 
  119, 124, 129, 134, 139, 144, 149, 154, 159, 164, 169, 174, 179, 184, 189, 194,
  199, 204, 209, 214, 219, 224, 229, 234, 239, 244, 249, 254, 256, 259, 261, 264, 
  269, 271, 276, 281, 285, 290, 295, 299, 304, 308, 310, 313, 317, 319, 324, 329, 
  334, 339, 344, 348, 353, 357, 362, 367, 372, 377, 382, 387, 390, 391, 394, 399
};

int g_update_screen;

int g_draw_cursor_x;
int g_draw_cursor_y;

int g_old_draw_cursor_x;
int g_old_draw_cursor_y;

int g_pic_render_x;
int g_pic_render_y;

int g_draw_position_x;
int g_draw_position_y;

int g_palette_page;
int g_cur_color;
int g_prev_color;

int g_play_area_w;
int g_play_area_h;

int g_mark_current;

int g_across_scrollbar_x;
int g_across_scrollbar_width;

int g_down_scrollbar_y;
int g_down_scrollbar_height;

BITMAP *g_numbers;
BITMAP *g_highlight_numbers;
BITMAP *g_bg_lower;
BITMAP *g_bg_right;
BITMAP *g_mainarea;
BITMAP *g_pal_col;
BITMAP *g_draw_cursor;
BITMAP *g_small_pal;
BITMAP *g_large_pal;
BITMAP *g_pal_cursor;
BITMAP *g_wrong;
BITMAP *g_page_buttons;
BITMAP *g_prop_font;

RenderComponents g_components;

/*=============================================================================
 * update_scrollbar_positions
 *============================================================================*/
void update_scrollbar_positions(void) {
    /* For the x scrollbar:
       - The x position = (percentage of picture at left side of area
         * width of bar)

         For example, a 320 width picture with pixel 72 at the left side of the
         play area:
             72 / 320 * 199 = 44.775 - round down to 44

        - The width = (percentage of picture displayed / width of bar)

         For the same example above:
             20 / 320 * 199 = 12.43 - round down to 12

         Width should be capped at scrollbar area width.

         If the width of the picture is <= play area width, draw a full
         width scrollbar
    */

   /* If the picture width is smaller than the screen width, draw a full 
      length scrollbar */

   float across_scroll_x, across_scroll_w, down_scroll_y, down_scroll_h;

   if (g_picture->h <= g_play_area_h) {
     g_down_scrollbar_y = 0;
     g_down_scrollbar_height = Y_SCROLLBAR_AREA_HEIGHT - 1;
   } 
   else {
     down_scroll_y = (float)g_pic_render_y / (float)g_picture->h *
                      Y_SCROLLBAR_AREA_HEIGHT;
     down_scroll_h = (float)g_play_area_h / (float)g_picture->h *
                      Y_SCROLLBAR_AREA_HEIGHT;

     g_down_scrollbar_y = (int)down_scroll_y;
     g_down_scrollbar_height = (int)down_scroll_h;
   }

   if (g_picture->w <= g_play_area_w) {
    g_across_scrollbar_x = 0;
    g_across_scrollbar_width = X_SCROLLBAR_AREA_WIDTH - 1;
   }
   else {
     across_scroll_x = (float)g_pic_render_x / (float)g_picture->w *
                        X_SCROLLBAR_AREA_WIDTH;
     across_scroll_w = (float)g_play_area_w / (float)g_picture->w *
                        X_SCROLLBAR_AREA_WIDTH;

     g_across_scrollbar_x = (int)across_scroll_x;                           
     g_across_scrollbar_width = (int)across_scroll_w;
   }

}

/*=============================================================================
 * render_scrollbars
 *============================================================================*/
void render_scrollbars(BITMAP *dest) {
  /* Clear the existing scrollbar areas */
  rectfill(dest, X_SCROLLBAR_AREA_X, X_SCROLLBAR_AREA_Y,
          X_SCROLLBAR_AREA_X + X_SCROLLBAR_AREA_WIDTH - 1,
          X_SCROLLBAR_AREA_Y + X_SCROLLBAR_AREA_HEIGHT - 1,
          SCROLLBAR_BG_COLOR);

  rectfill(dest, Y_SCROLLBAR_AREA_X, Y_SCROLLBAR_AREA_Y,
          Y_SCROLLBAR_AREA_X + Y_SCROLLBAR_AREA_WIDTH - 1,
          Y_SCROLLBAR_AREA_Y + Y_SCROLLBAR_AREA_HEIGHT - 1,
          SCROLLBAR_BG_COLOR);

  /* Draw the across scrollbar */
  rectfill(dest, X_SCROLLBAR_AREA_X + g_across_scrollbar_x, 
           X_SCROLLBAR_AREA_Y,
           g_across_scrollbar_x + g_across_scrollbar_width + 
           X_SCROLLBAR_AREA_X, 
           X_SCROLLBAR_AREA_Y + X_SCROLLBAR_AREA_HEIGHT - 1, 
           SCROLLBAR_INTERIOR_COLOR);
  rect(dest, 
       X_SCROLLBAR_AREA_X + g_across_scrollbar_x - 1, 
       X_SCROLLBAR_AREA_Y - 1,
       g_across_scrollbar_x + g_across_scrollbar_width + X_SCROLLBAR_AREA_X + 1, 
       X_SCROLLBAR_AREA_Y + X_SCROLLBAR_AREA_HEIGHT, 
       SCROLLBAR_BORDER_COLOR);

  /* Draw the down scrollbar */
  rectfill(dest, Y_SCROLLBAR_AREA_X, 
           Y_SCROLLBAR_AREA_Y + g_down_scrollbar_y,
           Y_SCROLLBAR_AREA_X + Y_SCROLLBAR_AREA_WIDTH - 1,
           g_down_scrollbar_y + g_down_scrollbar_height + Y_SCROLLBAR_AREA_Y,
           SCROLLBAR_INTERIOR_COLOR);  
  rect(dest, Y_SCROLLBAR_AREA_X - 1, 
       Y_SCROLLBAR_AREA_Y + g_down_scrollbar_y - 1,
       Y_SCROLLBAR_AREA_X + Y_SCROLLBAR_AREA_WIDTH,
       g_down_scrollbar_y + g_down_scrollbar_height + Y_SCROLLBAR_AREA_Y +1,
       SCROLLBAR_BORDER_COLOR);             
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
  c->render_palette_cursor = 0;
  c->render_scrollbars = 0;
  c->render_all = 0;
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
    if (g_cur_color == pal_offset & g_mark_current == 1) {
      blit(g_highlight_numbers, 
          dest, 
          pal_offset * NUMBER_BOX_WIDTH,
          0, 
          DRAW_AREA_X + (off_x * NUMBER_BOX_RENDER_X_OFFSET),
          DRAW_AREA_Y + (off_y * NUMBER_BOX_RENDER_Y_OFFSET),
          NUMBER_BOX_WIDTH, 
          NUMBER_BOX_HEIGHT);
    }
    else {
      blit(g_numbers, 
          dest, 
          pal_offset * NUMBER_BOX_WIDTH,
          0, 
          DRAW_AREA_X + (off_x * NUMBER_BOX_RENDER_X_OFFSET),
          DRAW_AREA_Y + (off_y * NUMBER_BOX_RENDER_Y_OFFSET),
          NUMBER_BOX_WIDTH, 
          NUMBER_BOX_HEIGHT);
    }
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
 * render_status_text
 *============================================================================*/
void render_status_text(BITMAP *dest) {
  char render_text[40];
  int hours, minutes, seconds;

  /* Clear the box entirely */
  rectfill(dest, 1, 170, 208, 198, 194);

  hours = g_elapsed_time / 3600;
  minutes = (g_elapsed_time - hours * 3600) / 60;
  seconds = (g_elapsed_time - hours * 3600) % 60;

  /* Render the category */
  sprintf(render_text, "Category : %s", g_categories[g_picture->category]);
  render_prop_text(dest, render_text, CATEGORY_TEXT_X, CATEGORY_TEXT_Y);    
  /* Render elapsed time */
  /* If over 1000 hours, just cap the display at 999:59:59 */
  /* if over 100 hours, use a 3 digit hour counter, otherwise use 2. */
  if (g_elapsed_time >= 3600000)
    sprintf(render_text, "Elapsed : 999:59:59");  
  else if(g_elapsed_time >= 360000)
    sprintf(render_text, "Time : %03d:%02d:%02d", hours, minutes, seconds);
  else 
    sprintf(render_text, "Time : %02d:%02d:%02d", hours, minutes, seconds);
  render_prop_text(dest, render_text, ELAPSED_TEXT_X, ELAPSED_TEXT_Y);
  /* Render picture size */
  sprintf(render_text, "Size: %dx%d", g_picture->w, g_picture->h);
  render_prop_text(dest, render_text, SIZE_TEXT_X, SIZE_TEXT_Y);
  /* Render mistake count */
  sprintf(render_text, "Mistakes : %d", g_mistake_count);
  render_prop_text(dest, render_text, MISTAKES_X, MISTAKES_Y);
  /* Render progress text */
  sprintf(render_text, "Progress : %d / %d   ", g_correct_count,
          g_total_picture_squares);
  render_prop_text(dest, render_text, PROGRESS_X, PROGRESS_Y);
}

/*=============================================================================
 * render_screen
 *============================================================================*/
void render_screen(BITMAP *dest, RenderComponents c) {
  int start_index, palette_color, square_x, square_y, area_w, area_h;
  int swatch_x, swatch_y, pal_index, pal_x, pal_y;
  int i, j;
  char render_text[40];

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
    render_status_text(dest);
  }

  /* Draw the squares in the play area */
  if(c.render_main_area_squares || c.render_all) {
    /* If the picture is smaller than the play area, only draw the smaller
       area */
    for(i = 0; i < g_play_area_w; i++) {
      for(j = 0; j < g_play_area_h; j++) {
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

  if(c.render_scrollbars || c.render_all ) {
    update_scrollbar_positions();
    render_scrollbars(dest);
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
    textprintf(dest, font, 5, 171, 209, "%d %d     ", g_down_scrollbar_y, g_down_scrollbar_height);    
    //textprintf(dest, font, 5, 181, 209, "g_pic_render_y = %d   ", g_pic_render_y);
  }

  /* Clear the render flags */
  clear_render_components(&g_components);
  g_update_screen = 0;

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
 * render_prop_text
 *============================================================================*/
void render_prop_text(BITMAP *dest, char *text, int x_pos, int y_pos) {
	int x;
	int offset;
	char *cur;

  cur = text;
  x = x_pos;

	while (*cur != 0) {
		offset = (*cur++) - 32;
		masked_blit(g_prop_font, dest, g_prop_font_offset[offset],
		     0, x, y_pos, g_prop_font_width[offset], g_prop_font_height);
		x += g_prop_font_width[offset] + 1;
	}
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
  g_highlight_numbers  = load_pcx("RES/NUMS_HI.PCX", res_pal);
  if(g_highlight_numbers == NULL) {
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
  destroy_bitmap(g_highlight_numbers);
  destroy_bitmap(g_bg_lower);
  destroy_bitmap(g_bg_right);
  destroy_bitmap(g_mainarea);
  destroy_bitmap(g_draw_cursor);
  destroy_bitmap(g_small_pal);
  destroy_bitmap(g_large_pal);
  destroy_bitmap(g_pal_cursor);
  destroy_bitmap(g_wrong);
  destroy_bitmap(g_page_buttons);
  destroy_bitmap(g_prop_font);
}
