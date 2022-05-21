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
#include <math.h>
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

int g_show_map_text;

int g_draw_style;

int g_help_page;

int g_replay_scale;

BITMAP *g_logo;

BITMAP *g_title_area;
BITMAP *g_title_box;

BITMAP *g_numbers;
BITMAP *g_highlight_numbers;
BITMAP *g_bg_lower;
BITMAP *g_bg_right;
BITMAP *g_mainarea;
BITMAP *g_pal_col;
BITMAP *g_draw_cursor;
BITMAP *g_small_pal;
BITMAP *g_large_pal;
BITMAP *g_large_diamonds;
BITMAP *g_large_crosses;
BITMAP *g_pal_cursor;
BITMAP *g_wrong;
BITMAP *g_page_buttons;
BITMAP *g_main_buttons;
BITMAP *g_prop_font;
BITMAP *g_save_notice;
BITMAP *g_load_notice;
BITMAP *g_load_dialog;
BITMAP *g_overview_box;
BITMAP *g_overview_cursor;
BITMAP *g_finished_dialog;
BITMAP *g_mouse_cursor;
BITMAP *g_help_next;
BITMAP *g_help_previous;
BITMAP *g_help_exit;

RenderComponents g_components;
TitleAnimation g_title_anim;

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
     down_scroll_y = ((float)g_pic_render_y / (float)g_picture->h) *
                      (float)Y_SCROLLBAR_AREA_HEIGHT;
     down_scroll_h = ((float)g_play_area_h / (float)g_picture->h) *
                      (float)Y_SCROLLBAR_AREA_HEIGHT;
     g_down_scrollbar_y = floor(down_scroll_y);
     g_down_scrollbar_height = floor(down_scroll_h);
   }

   if (g_picture->w <= g_play_area_w) {
    g_across_scrollbar_x = 0;
    g_across_scrollbar_width = X_SCROLLBAR_AREA_WIDTH - 1;
   }
   else {
     across_scroll_x = (g_pic_render_x / (float)g_picture->w) *
                        X_SCROLLBAR_AREA_WIDTH;
     across_scroll_w = (g_play_area_w / (float)g_picture->w) *
                       X_SCROLLBAR_AREA_WIDTH;

     g_across_scrollbar_x = floor(across_scroll_x);
     g_across_scrollbar_width = floor(across_scroll_w);
   }
}

/*=============================================================================
 * render_logo
 *============================================================================*/
void render_logo(BITMAP *dest, RenderComponents c) {
  blit(g_logo, dest, 0, 0, 0, 0, g_logo->w, g_logo->h);
}

/*=============================================================================
 * render_title_screen
 *============================================================================*/
void render_title_screen(BITMAP *dest, RenderComponents c) {
  int i,j;
  if(g_title_anim.update_background == 1) {      
    set_palette(title_pal);      
    clear_to_color(g_title_area, 208);    
    for(i=0;i<32;i++) {
      for (j=3; j<17; j++) {
        blit(g_numbers, g_title_area, 
             (rand() % 64 + 1) * NUMBER_BOX_WIDTH, 
             0,
             i * (NUMBER_BOX_WIDTH - 1),
             j * (NUMBER_BOX_HEIGHT - 1),
             NUMBER_BOX_WIDTH,
             NUMBER_BOX_HEIGHT);
      }
    }
    hline(g_title_area, 0, 29, 319, 205);
    hline(g_title_area, 0, 171, 319, 205);

    render_centered_prop_text(g_title_area, "Copyright 2022 Shaun Brandt / Holy Meatgoat Productions", 160, 12);
    render_centered_prop_text(g_title_area, "-- Press ENTER or click to play! --", 160, 182);    
    g_title_anim.update_background = 0;
  }

  if(g_title_anim.update_title_color == 1 && g_title_anim.color_start == 1) {
    for (i=0; i< 40; i++) {
      blit(g_large_pal, g_title_area,
        (rand() % 64 + 1) * NUMBER_BOX_WIDTH,
        0,
        (rand() % 32) * (NUMBER_BOX_WIDTH - 1),
        ((rand() % 14) + 3) * (NUMBER_BOX_HEIGHT - 1),
        NUMBER_BOX_WIDTH,
        NUMBER_BOX_HEIGHT);
    }
    g_title_anim.update_title_color = 0;
  }

  blit(g_title_box, g_title_area, 0, 0, 80, 60, g_title_box->w, g_title_box->h);
  blit(g_title_area, dest, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
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
  c->render_buttons = 0;
  c->render_overview_display = 0;
  c->render_status_text = 0;
  c->render_draw_cursor = 0;
  c->render_palette_cursor = 0;
  c->render_scrollbars = 0;
  c->render_map = 0;
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

  BITMAP *draw_style;

  /* Pick the draw style */
  switch (g_draw_style) {
    case STYLE_SOLID:
      draw_style = g_large_pal;
      break;
    case STYLE_DIAMOND:
      draw_style = g_large_diamonds;
      break;
    case STYLE_CROSS:
      draw_style = g_large_crosses;
      break;
    default:
      draw_style = g_large_pal;
      break;
  }
  c = g_picture->pic_squares[(tl_y + off_y) * g_picture->w + (tl_x +off_x)];
  pal_offset = c.pal_entry;  
  color_offset = c.fill_value;

  if(color_offset == 0) {
    if (g_cur_color == pal_offset && g_mark_current == 1) {
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
    blit(draw_style, 
        dest, 
        color_offset * NUMBER_BOX_WIDTH,
        0, 
        DRAW_AREA_X + (off_x * NUMBER_BOX_RENDER_X_OFFSET),
        DRAW_AREA_Y + (off_y * NUMBER_BOX_RENDER_Y_OFFSET),
        NUMBER_BOX_WIDTH, 
        NUMBER_BOX_HEIGHT);    
    /* Is it marked with the correct color?  If not, draw an X on it */
    if (!c.correct) {
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
  sprintf(render_text, "Progress : %d/%d   ", g_correct_count,
          g_total_picture_squares);
  render_prop_text(dest, render_text, PROGRESS_X, PROGRESS_Y);
}

/*=============================================================================
 * render_primary_ui
 *============================================================================*/
void render_primary_ui(BITMAP *dest) {
  /* Draw the background parts */
    blit(g_bg_right, dest, 0, 0, RIGHT_SIDE_PANEL_X, RIGHT_SIDE_PANEL_Y,
         g_bg_right->w, g_bg_right->h);
    blit(g_bg_lower, dest, 0, 0, BOTTOM_PANEL_X, BOTTOM_PANEL_Y,
         g_bg_lower->w, g_bg_lower->h);
    blit(g_mainarea, dest, 0, 0, MAIN_AREA_X, MAIN_AREA_Y, 
         g_mainarea->w, g_mainarea->h);

}

/*=============================================================================
 * render_menu_buttons
 *============================================================================*/
void render_menu_buttons(BITMAP *dest) {
  /* Draw the buttons */

  /* Save */
  if (g_highlight_save_button == 0) {
    blit(g_main_buttons, dest, SAVE_BUTTON_X_OFFSET, SAVE_BUTTON_Y_OFFSET,
         SAVE_BUTTON_X, SAVE_BUTTON_Y, MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT);
  }
  else {
    blit(g_main_buttons, dest, SAVE_BUTTON_X_OFFSET, BUTTON_PRESSED_OFFSET + SAVE_BUTTON_Y_OFFSET,
         SAVE_BUTTON_X, SAVE_BUTTON_Y, MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT);    
  }

  /* Style */
  if(g_highlight_style_button == 0) {
    blit(g_main_buttons, dest, STYLE_BUTTON_X_OFFSET, STYLE_BUTTON_Y_OFFSET,
        STYLE_BUTTON_X, STYLE_BUTTON_Y, MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT);
  }
  else {
      blit(g_main_buttons, dest, STYLE_BUTTON_X_OFFSET, BUTTON_PRESSED_OFFSET + STYLE_BUTTON_Y_OFFSET,
        STYLE_BUTTON_X, STYLE_BUTTON_Y, MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT);
  }

  /* Options */
  blit(g_main_buttons, dest, OPTS_BUTTON_X_OFFSET, OPTS_BUTTON_Y_OFFSET,
       OPTS_BUTTON_X, OPTS_BUTTON_Y, MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT);

  /* Help */
  blit(g_main_buttons, dest, HELP_BUTTON_X_OFFSET, HELP_BUTTON_Y_OFFSET,
       HELP_BUTTON_X, HELP_BUTTON_Y, MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT);

  /* Load */
  if (g_highlight_load_button == 0) {
    blit(g_main_buttons, dest, LOAD_BUTTON_X_OFFSET, LOAD_BUTTON_Y_OFFSET,
         LOAD_BUTTON_X, LOAD_BUTTON_Y, MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT);   
  }
  else {
    blit(g_main_buttons, dest, LOAD_BUTTON_X_OFFSET, BUTTON_PRESSED_OFFSET + LOAD_BUTTON_Y_OFFSET,
         LOAD_BUTTON_X, LOAD_BUTTON_Y, MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT);      
  }

  /* Mark */
  if(g_mark_current == 1) {
    blit(g_main_buttons, dest, MARK_BUTTON_X_OFFSET, 
         BUTTON_PRESSED_OFFSET + MARK_BUTTON_Y_OFFSET, 
         MARK_BUTTON_X, MARK_BUTTON_Y, MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT);           
  }
  else {
    blit(g_main_buttons, dest, MARK_BUTTON_X_OFFSET, 
         BUTTON_DEFAULT_OFFSET + MARK_BUTTON_Y_OFFSET, 
         MARK_BUTTON_X, MARK_BUTTON_Y, MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT);      
  }  

  /* Map */
  blit(g_main_buttons, dest, MAP_BUTTON_X_OFFSET, MAP_BUTTON_Y_OFFSET,
       MAP_BUTTON_X, MAP_BUTTON_Y, MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT);  

  /* Exit */
  blit(g_main_buttons, dest, EXIT_BUTTON_X_OFFSET, EXIT_BUTTON_Y_OFFSET,
      EXIT_BUTTON_X, EXIT_BUTTON_Y, MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT);          
}

/*=============================================================================
 * render_map_screen
 *============================================================================*/
void render_map_screen(BITMAP *dest, RenderComponents c) {
    int i, j, x_pos, y_pos, color, actual_color, row_1_width,
        row_2_width, exit_width, center, box_width;
    char text[40], text2[40];

    if(c.render_map) {
      x_pos = (SCREEN_W - g_picture->w) / 2;
      y_pos = (SCREEN_H - g_picture->h) / 2;

      /* Clear the map area */
      clear_to_color(dest, 194);

      /* Draw the pixels if colored, or background color if not */
      for(j=0; j<g_picture->h; j++) {
        for(i=0; i<g_picture->w; i++) {
          color = g_picture->pic_squares[j * g_picture->w + i].fill_value;
          actual_color = g_picture->pic_squares[j * g_picture->w + i].pal_entry;
          if (color != 0 && color == actual_color) {
            putpixel(dest, x_pos + i, y_pos + j, color - 1);
          }
          else {
            putpixel(dest, x_pos + i, y_pos + j, 208);
          }
        }
      }

      /* If the picture is smaller than the screen, draw a border */
      if(g_picture->w < SCREEN_W && g_picture->h < SCREEN_H) {
        rect(dest, x_pos - 1, y_pos - 1, 
            x_pos + g_picture->w, y_pos + g_picture->h, 203);
      }

      /* Display the map text if requested */
      if(g_show_map_text == 1) {
        center = SCREEN_W / 2;
        
        sprintf(text, "Click or press C to toggle this message");
        sprintf(text2, "Press Exit button or M to exit map mode");
        row_1_width = get_prop_text_width(text);
        row_2_width = get_prop_text_width(text2);
      
        box_width = (row_1_width > row_2_width) ? row_1_width : row_2_width;

        rectfill(dest, center - (box_width/2) - 3, 163,
                center + (box_width/2), 181, 208);
  
        rect(dest, center - (box_width/2) - 3, 163,
             center + (box_width/2) + 2, 182, 203);

        render_centered_prop_text(dest, text, SCREEN_W / 2, 165); 
        render_centered_prop_text(dest, text2, SCREEN_W / 2, 174);             

        sprintf(text, "Exit");
        exit_width = get_prop_text_width(text);

        rectfill(dest, center - (exit_width/2) - 20, 186,
                center + (exit_width/2) + 17, 194, 208);  
        rect(dest, center - (exit_width/2) - 20, 186,
             center + (exit_width/2) + 17, 195, 203);        

        render_centered_prop_text(dest, text, SCREEN_W / 2, 188);    
      } 
      clear_render_components(&g_components);
    }
}

/*=============================================================================
 * render_game_screen
 *============================================================================*/
void render_game_screen(BITMAP *dest, RenderComponents c) {
  int start_index, pal_index, pal_x, pal_y, i, j;

  /* Draw the static UI components */
  if (c.render_ui_components || c.render_all) {  
    render_primary_ui(dest);
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
    blit(g_overview_box, dest, 0, 0, OVERVIEW_X, OVERVIEW_Y,
         g_overview_box->w, g_overview_box->h);

    /* Draw a border around the window (to hide any cursor movement) */
    rect(dest, OVERVIEW_X - 1, OVERVIEW_Y - 1, OVERVIEW_X + OVERVIEW_WIDTH, OVERVIEW_Y + OVERVIEW_HEIGHT, 207);

    /* Draw the cursor around the currently active region of the overview
       screen */ 
    draw_sprite(dest, g_overview_cursor, 
                OVERVIEW_X - 1 + g_pic_render_x / OVERVIEW_BLOCK_SIZE,
                OVERVIEW_Y - 1 + g_pic_render_y / OVERVIEW_BLOCK_SIZE);
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

  if(c.render_buttons | c.render_all ) {
    render_menu_buttons(dest);
  }

 
  if(c.render_scrollbars || c.render_all ) {
    // TODO: fix the update_scrollbar_positions function
    update_scrollbar_positions();
    render_scrollbars(dest);
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
    /* Add debug stuff here */
  }
}

/*=============================================================================
 * render_screen
 *============================================================================*/
void render_screen(BITMAP *dest, RenderComponents c) {

  switch(g_state) {
    case STATE_LOGO:
      render_logo(dest, c);
      break;
    case STATE_TITLE:
      render_title_screen(dest, c);
      break;
    case STATE_GAME:
      render_game_screen(dest, c);
      break;
    case STATE_MAP:
      render_map_screen(dest, c);
      break;
    case STATE_LOAD:
      render_load_message(dest, c);
      break;
    case STATE_SAVE:
      render_save_message(dest, c);
      break;
    case STATE_HELP:
      render_help_text(dest, c);
      break;
    case STATE_OPTS:
      break;
    case STATE_LOAD_DIALOG:
      render_load_dialog(dest, c);
      break;
    case STATE_FINISHED:
      break;
    case STATE_REPLAY:
      render_replay_state(dest, c);
      break;
    default:
      break;
  }

  /* Clear the render flags */
  clear_render_components(&g_components);
}

/*=============================================================================
 * render_help_text
 *============================================================================*/
void render_help_text(BITMAP *dest, RenderComponents c) {

  /* Fill in the background */
  clear_to_color(dest, 194);
  rect(dest, 0, 0, 319, 199, 205 );
  rect(dest, 1, 1, 318, 198, 208 );

  switch(g_help_page) {
    case 0:
      render_centered_prop_text(dest, "Welcome to Damaniel's Pixel by Number!", 160, 5);
      render_centered_prop_text(dest, "(The most useless retro 'game' ever made!)", 160, 13);      
      
      render_prop_text(dest, "This 'game' (if you want to call it that) is a hyper-casual", 8, 28);   
      render_prop_text(dest, "paint by number thing.  Think 'old-school physical paint by", 8, 36);   
      render_prop_text(dest, "number canvas' or 'modern diamond painting', but in a digital", 8, 44);   
      render_prop_text(dest, "form for a totally obsolete OS!  Fun.", 8, 52);  

      render_prop_text(dest, "The entire goal is to fill in every square of one or more of", 8, 68);  
      render_prop_text(dest, "the provided picture files with the correct color,", 8, 76);  
      render_prop_text(dest, "re-creating the original image file, one pixel at a time.", 8, 84);  

      render_prop_text(dest, "The most relevant keyboard controls are:", 8, 92);
      render_prop_text(dest, "- Arrow keys: move the cursor one square", 50, 108);
      render_prop_text(dest, "- SHIFT + arrow keys: move the cursor one page", 50, 118);
      render_prop_text(dest, "- [ / ]: Move up and down through the palette", 50, 128);
      render_prop_text(dest, "- P: Switch palette pages", 50, 138);
      render_prop_text(dest, "- Space: color a square in!", 50, 148);      

      render_prop_text(dest, "See the following page for a list of every command.", 8, 164);
      render_prop_text(dest, "Some are extremely useful!.", 8, 172);

      draw_sprite(dest, g_help_exit, 117, 185);
      draw_sprite(dest, g_help_next, 220, 185);      
      break;
    case 1:
      render_centered_prop_text(dest, "Complete List of Controls", 160, 8);

      render_prop_text(dest, "Arrow keys :  move the cursor one square", 8, 24); 
      render_prop_text(dest, "SHIFT + arrow keys :  move the cursor one page", 8, 34); 
      render_prop_text(dest, "[ / ] : Move up and down through the palette", 8, 44);
      render_prop_text(dest, "P : Switch palette pages", 8, 54);
      render_prop_text(dest, "Space : color a square in!", 8, 64);      
      render_prop_text(dest, "Space will also clear an incorrectly colored square", 24, 74);
      render_prop_text(dest, "M : show a map of overall progress", 8, 84);
      render_prop_text(dest, "K : Highlight all squares of the current color", 8, 94);
      render_prop_text(dest, "T : Change the drawing style", 8, 104);      
      render_prop_text(dest, "S : Save your progress", 8, 114);  
      render_prop_text(dest, "L : Load a new picture", 8, 124);
      render_prop_text(dest, "H : Help (you must have discovered this one already!)", 8, 134);
      render_prop_text(dest, "ESC : Return to title, or exit the Help menu", 8, 144);      

      draw_sprite(dest, g_help_previous, 15, 185);
      draw_sprite(dest, g_help_exit, 117, 185);
      draw_sprite(dest, g_help_next, 220, 185);     
      break;
    case 2:
      render_centered_prop_text(dest, "Explanation of features", 160, 5);

      render_prop_text(dest, "Highlighting the active color:", 8, 20);           
      render_prop_text(dest, "  Pressing the K key toggles highlighting of the", 8, 30);
      render_prop_text(dest, "  current color.  In this mode, unmarked squares of the", 8, 38);  
      render_prop_text(dest, "  active color will be displayed in gray instead of white,", 8, 46);
      render_prop_text(dest, "  making them easier to find.", 8, 54);  

      render_prop_text(dest, "The progress map:", 8, 70); 
      render_prop_text(dest, "  Pressing the M key will bring up the progress map. This", 8, 80);
      render_prop_text(dest, "  mode shows an overview of the entire picture, giving a ", 8, 88);
      render_prop_text(dest, "  detailed overview of overall progress.  Press C to ", 8, 96);
      render_prop_text(dest, "  toggle the help message on the map screen and M to ", 8, 104);
      render_prop_text(dest, "  return to the game.", 8, 112);

      render_prop_text(dest, "The palette:", 8, 128); 
      render_prop_text(dest, "  A picture can consist of up to 64 different colors.", 8, 138);      
      render_prop_text(dest, "  The bracket keys ([ and ]) are used to move up and", 8, 146);      
      render_prop_text(dest, "  down through colors in the palette.  For pictures with", 8, 154);
      render_prop_text(dest, "  more than 32 colors, the additional colors are on a ", 8, 162);
      render_prop_text(dest, "  second page.  Press P to switch between pages.", 8, 170);

      draw_sprite(dest, g_help_previous, 15, 185);
      draw_sprite(dest, g_help_exit, 117, 185);
      draw_sprite(dest, g_help_next, 220, 185);    
      break;
    case 3:
      render_centered_prop_text(dest, "Explanation of (more) features", 160, 8);    

      render_prop_text(dest, "The overview window:", 8, 24);           
      render_prop_text(dest, "  The box in the upper right corner of the screen is a", 8, 34);
      render_prop_text(dest, "  high level overview of current progress.  The yellow box", 8, 42);  
      render_prop_text(dest, "  inside it represents the visible part of the picture as", 8, 50);  
      render_prop_text(dest, "  shown in the main area. Each 'pixel' in this box ", 8, 58);
      render_prop_text(dest, "  represents a 4x4 square block of the final image. ", 8, 66);

      render_prop_text(dest, "  The colors used are as follows:", 8, 82); 
      render_prop_text(dest, "    - White : every square in this region is done", 8, 92);
      render_prop_text(dest, "    - Blue :  some squares this region are done", 8, 100);
      render_prop_text(dest, "    - Black : no squares in this region are done ", 8, 108);
      render_prop_text(dest, "    - Red : one or more squares was filled in wrong", 8, 116);

      render_prop_text(dest, "Mistakes:", 8, 132);           
      render_prop_text(dest, "  If a square is incorrectly filled in, it will be shown", 8, 142);
      render_prop_text(dest, "  with an 'X' over it, and the relevant region of the", 8, 150);
      render_prop_text(dest, "  overview window will show a red pixel.  To fix it, just", 8, 158);
      render_prop_text(dest, "  use the Space bar to remove the offending square.", 8, 166);

      draw_sprite(dest, g_help_previous, 15, 185);
      draw_sprite(dest, g_help_exit, 117, 185);
      draw_sprite(dest, g_help_next, 220, 185);     
      break;
    case 4:
      render_centered_prop_text(dest, "Explanation of (yet more) features", 160, 8);
      
      render_prop_text(dest, "Styles:", 8, 24);
      render_prop_text(dest, "  The individual squares in the main play area can be", 8, 34);
      render_prop_text(dest, "  displayed in one of 3 different ways.  The T key", 8, 42);  
      render_prop_text(dest, "  is used to switch between styles.", 8, 50);

      render_prop_text(dest, "  The available styles are:", 8, 66); 
      render_prop_text(dest, "    - Solid : the default.  Each square is a solid color", 8, 76);
      render_prop_text(dest, "    - Diamond : each square has a raised effect", 8, 86);
      render_prop_text(dest, "    - X : each square looks kind of like a cross stitch ", 8, 96);      

      render_prop_text(dest, "Saving and loading:", 8, 112); 
      render_prop_text(dest, "  Press the S key to save your progress.", 8, 122);
      render_prop_text(dest, "  Press the L key to bring up the load menu.", 8, 138);
      render_prop_text(dest, "  In the load menu, press Up and down to select an image.", 8, 146);
      render_prop_text(dest, "  Press Enter to select the image, or Esc to cancel.", 8, 154);

      draw_sprite(dest, g_help_previous, 15, 185);
      draw_sprite(dest, g_help_exit, 117, 185);
      draw_sprite(dest, g_help_next, 220, 185);      
      break;
    case 5:
      render_centered_prop_text(dest, "Mouse support (finally!)", 160, 8);
      
      render_prop_text(dest, "Mouse support is now available!  This page describes", 8, 24);
      render_prop_text(dest, "key mouse-specific features.", 8, 32);

      render_prop_text(dest, "Drawing and erasing in the play area:", 8, 48);  
      render_prop_text(dest, "  To fill in an empty square in the play area, click", 8, 58);
      render_prop_text(dest, "  on it.  If you hold down the mouse and move, you can", 8, 66); 
      render_prop_text(dest, "  fill in any squares the mouse moves over.", 8, 74);
      render_prop_text(dest, "  To erase a mistake, highlight it and click.  Like", 8, 82);
      render_prop_text(dest, "  drawing, you can hold down the mouse and move to  ", 8, 90);      
      render_prop_text(dest, "  erase any mistakes under the mouse cursor.  ", 8, 98);    
      render_prop_text(dest, "  (Note that keyboard controls work even in mouse mode.)", 8, 106);

      render_prop_text(dest, "Using the overview area to move the draw area:", 8, 122); 
      render_prop_text(dest, "  By clicking in the overview area, you can move the draw", 8, 132);
      render_prop_text(dest, "  area to match the region you clicked on.  Like drawing", 8, 140);
      render_prop_text(dest, "  and erasing, you can click and drag to scroll", 8, 148);
      render_prop_text(dest, "  through the entire play area.", 8, 156);

      draw_sprite(dest, g_help_previous, 15, 185);
      draw_sprite(dest, g_help_exit, 117, 185);
      draw_sprite(dest, g_help_next, 220, 185);      
      break;      
    case 6:
      render_centered_prop_text(dest, "About this program and the author", 160, 8);
      render_prop_text(dest, "  Damaniel's Paint by Number is copyright 2021-2022 by", 8, 24);
      render_prop_text(dest, "  Shaun Brandt and his weird company, Holy Meatgoat", 8, 32);  
      render_prop_text(dest, "  Productions.", 8, 40);  

      render_prop_text(dest, "  The source code is available under the MIT License - see", 8, 56);  
      render_prop_text(dest, "  the LICENSE file for more information. ", 8, 64); 

      render_prop_text(dest, "  More DOS-compatible software can be found here:", 8, 80);
      render_centered_prop_text(dest, "https://github.com/Damaniel/dos_games", 160, 96);

      render_prop_text(dest, "  Enjoy, I guess.  I put this together in a couple weeks", 8, 112);  
      render_prop_text(dest, "  after spending far too long working on real diamond", 8, 120);
      render_prop_text(dest, "  paintings and needed a break.", 8, 128);

      render_prop_text(dest, "  (Be glad I didn't do the text mode version I was joking", 8, 144); 
      render_prop_text(dest, "  about with my wife when I was 30 hours into this one...", 8, 152);

      render_centered_prop_text(dest, "...yet.)", 160, 168);

      draw_sprite(dest, g_help_previous, 15, 185);
      draw_sprite(dest, g_help_exit, 117, 185);  
      break;
    default:
      break;
  }
}

/*=============================================================================
 * render_load_dialog
 *============================================================================*/
void render_load_dialog(BITMAP *dest, RenderComponents c) {
  int start_offset, end_offset;
  int collection_start_offset, collection_end_offset;
  int i;
  char text[30];

  /* If the load dialog was invoked from the title screen, keep drawing 
     the title screen parts */
  if (g_prev_state == STATE_TITLE) {
    render_title_screen(dest, c);
    /* Cover the 'press key to play' box */
    rectfill(dest, 0, 180, 319, 190, 208);       
  }

  draw_sprite(dest, g_load_dialog, LOAD_DIALOG_X, LOAD_DIALOG_Y);

  /*---------------------------------------------------------------------
   * Render the collection list 
   *---------------------------------------------------------------------*/

  /* If there are collections, list them */
  if (g_num_collections > 0) {
    collection_start_offset = g_load_collection_offset;
    collection_end_offset = collection_start_offset + LOAD_NUM_VISIBLE_FILES;

    if (collection_end_offset > g_num_collections)
      collection_end_offset = g_num_collections;

    /* If the collection list is active, highlight the active item on the 
    * collection side and place a highlight aaround the section */
    if (g_load_section_active == LOAD_COLLECTION_ACTIVE) {
      rectfill(dest,
              LOAD_COLLECTION_NAME_X_OFF,
              LOAD_COLLECTION_NAME_Y_OFF + 
              (g_load_collection_cursor_offset * LOAD_COLLECTION_NAME_HEIGHT),
              LOAD_COLLECTION_NAME_X_OFF + LOAD_COLLECTION_NAME_WIDTH - 1,
              LOAD_COLLECTION_NAME_Y_OFF + 
              ((g_load_collection_cursor_offset+1) * LOAD_COLLECTION_NAME_HEIGHT) - 1, 204);
      rect(dest,
          COLLECTION_HIGHLIGHT_X_OFF,
          COLLECTION_HIGHLIGHT_Y_OFF,
          COLLECTION_HIGHLIGHT_X_OFF + COLLECTION_HIGHLIGHT_WIDTH -1,
          COLLECTION_HIGHLIGHT_Y_OFF + COLLECTION_HIGHLIGHT_HEIGHT -1, 210);             
    } else {    
      rect(dest,
          COLLECTION_HIGHLIGHT_X_OFF,
          COLLECTION_HIGHLIGHT_Y_OFF,
          COLLECTION_HIGHLIGHT_X_OFF + COLLECTION_HIGHLIGHT_WIDTH -1,
          COLLECTION_HIGHLIGHT_Y_OFF + COLLECTION_HIGHLIGHT_HEIGHT -1, 194);
    } 

    /* Iterate through and draw the file list */
    for(i=collection_start_offset; i < collection_end_offset ; i++) {
      /* Draw the file name */
      render_prop_text(dest, g_collection_items[i].name, LOAD_COLLECTION_NAME_X_OFF + 1,
                       LOAD_COLLECTION_NAME_Y_OFF + 
                      ((i-collection_start_offset) * LOAD_COLLECTION_NAME_HEIGHT) + 1);       
    }
  } else {
    /* If there aren't any collections, display a message that says that to
       the user, and don't render anything. */
    render_prop_text(dest, "- NONE -", LOAD_COLLECTION_NAME_X_OFF + 1,
                     LOAD_COLLECTION_NAME_Y_OFF + 1);
  }

  /*---------------------------------------------------------------------
   * Render the image list 
   *---------------------------------------------------------------------*/
  /* Figure out what picture offset to draw FROM */
  start_offset = g_load_picture_offset;
  end_offset = start_offset + LOAD_NUM_VISIBLE_FILES;

  if(end_offset > g_num_picture_files)
    end_offset = g_num_picture_files;

  /* Draw the background of the highlighted image, but only if the 
   * image section is currently active */
  if(g_load_section_active == LOAD_IMAGE_ACTIVE) {  
    rectfill(dest,
            LOAD_FILE_NAME_X_OFF,
            LOAD_FILE_NAME_Y_OFF + 
             (g_load_cursor_offset * LOAD_FILE_NAME_HEIGHT),
            LOAD_FILE_NAME_X_OFF + LOAD_FILE_NAME_WIDTH - 1,
            LOAD_FILE_NAME_Y_OFF + 
            ((g_load_cursor_offset+1) * LOAD_FILE_NAME_HEIGHT) - 1, 204);
    rect(dest,
         IMAGE_HIGHLIGHT_X_OFF,
         IMAGE_HIGHLIGHT_Y_OFF,
         IMAGE_HIGHLIGHT_X_OFF + IMAGE_HIGHLIGHT_WIDTH -1,
         IMAGE_HIGHLIGHT_Y_OFF + IMAGE_HIGHLIGHT_HEIGHT -1, 210);              
  } else {
    rect(dest,
         IMAGE_HIGHLIGHT_X_OFF,
         IMAGE_HIGHLIGHT_Y_OFF,
         IMAGE_HIGHLIGHT_X_OFF + IMAGE_HIGHLIGHT_WIDTH -1,
         IMAGE_HIGHLIGHT_Y_OFF + IMAGE_HIGHLIGHT_HEIGHT -1, 194);              
  }

  /* Iterate through and draw the file list */
  for(i=start_offset; i < end_offset ; i++) {
    /* Draw the file name */
    render_prop_text(dest, g_pic_items[i].name, LOAD_FILE_NAME_X_OFF + 1,
                   LOAD_FILE_NAME_Y_OFF + 
                   ((i-start_offset) * LOAD_FILE_NAME_HEIGHT) + 1);       
  }
    
  /*---------------------------------------------------------------------
   * Render the metadata
   *---------------------------------------------------------------------*/    
  
  /* Draw the category */
  rectfill(dest,
           LOAD_FILE_CATEGORY_X,
           LOAD_FILE_CATEGORY_Y,
           LOAD_FILE_CATEGORY_X + LOAD_FILE_CATEGORY_WIDTH - 1,
           LOAD_FILE_CATEGORY_Y + LOAD_FILE_CATEGORY_HEIGHT - 1, 
           208);
  render_centered_prop_text(dest, 
                     g_categories[(int)g_pic_items[g_load_picture_index].category],
                     LOAD_FILE_CATEGORY_TEXT_X, LOAD_FILE_CATEGORY_TEXT_Y);

  /* Draw the dimensions */
  rectfill(dest,
           LOAD_FILE_XSIZE_X,
           LOAD_FILE_XSIZE_Y,
           LOAD_FILE_XSIZE_X + LOAD_FILE_XSIZE_WIDTH - 1,
           LOAD_FILE_XSIZE_Y + LOAD_FILE_XSIZE_HEIGHT - 1, 
           208);  
  rectfill(dest,
           LOAD_FILE_YSIZE_X,
           LOAD_FILE_YSIZE_Y,
           LOAD_FILE_YSIZE_X + LOAD_FILE_YSIZE_WIDTH - 1,
           LOAD_FILE_YSIZE_Y + LOAD_FILE_YSIZE_HEIGHT - 1, 
           208);  
  sprintf(text, "%d", g_pic_items[g_load_picture_index].width);
  render_centered_prop_text(dest, text,
                            LOAD_FILE_XSIZE_TEXT_X, LOAD_FILE_XSIZE_TEXT_Y);
  sprintf(text, "%d", g_pic_items[g_load_picture_index].height);
  render_centered_prop_text(dest, text,
                           LOAD_FILE_YSIZE_TEXT_X, LOAD_FILE_YSIZE_TEXT_Y);

  /* Draw the colors */
  rectfill(dest,
           LOAD_FILE_COLORS_X,
           LOAD_FILE_COLORS_Y,
           LOAD_FILE_COLORS_X + LOAD_FILE_COLORS_WIDTH - 1,
           LOAD_FILE_COLORS_Y + LOAD_FILE_COLORS_HEIGHT - 1, 
           208);  
  sprintf(text, "%d", g_pic_items[g_load_picture_index].colors);
  render_centered_prop_text(dest, text,
                     LOAD_FILE_COLORS_TEXT_X, LOAD_FILE_COLORS_TEXT_Y);

  /* Draw the progress */
  rectfill(dest,
           LOAD_FILE_PROGRESS_X,
           LOAD_FILE_PROGRESS_Y,
           LOAD_FILE_PROGRESS_X + LOAD_FILE_PROGRESS_WIDTH - 1,
           LOAD_FILE_PROGRESS_Y + LOAD_FILE_PROGRESS_HEIGHT - 1, 
           208);  
  if(g_pic_items[g_load_picture_index].progress == 0) 
    sprintf(text, "None yet");
  else 
    sprintf(text, "%d/%d", g_pic_items[g_load_picture_index].progress,
            g_pic_items[g_load_picture_index].width *
            g_pic_items[g_load_picture_index].height);
    
  render_centered_prop_text(dest, text,
                            LOAD_FILE_PROGRESS_TEXT_X, 
                            LOAD_FILE_PROGRESS_TEXT_Y);                         
}

/*=============================================================================
 * render_save_message
 *============================================================================*/
void render_save_message(BITMAP *dest, RenderComponents c) {
  draw_sprite(dest, g_save_notice, SAVING_MESSAGE_X, SAVING_MESSAGE_Y);
}

/*=============================================================================
 * render_load_message
 *============================================================================*/
void render_load_message(BITMAP *dest, RenderComponents c) {
  draw_sprite(dest, g_load_notice, LOADING_MESSAGE_X, LOADING_MESSAGE_Y);
}

/*=============================================================================
 * render_replay_state
 *============================================================================*/
void render_replay_state(BITMAP *dest, RenderComponents c) {
  int i;
  int color, x, y;

  if(g_replay_first_time == 1) {
    clear_to_color(dest, 209);  
    g_replay_first_time = 0;
  }

  for(i=g_replay_total; i< g_replay_total + g_replay_increment; i++) {
    if (i<g_total_picture_squares) {
      color = g_picture->pic_squares[g_picture->draw_order[i].y *
                                     g_picture->w + 
                                     g_picture->draw_order[i].x].pal_entry - 1;
      x = g_replay_x + (g_picture->draw_order[i].x * g_replay_scale);
      y = g_replay_y + (g_picture->draw_order[i].y * g_replay_scale);
      rectfill(dest, x, y, x+g_replay_scale-1, y+g_replay_scale-1, color);
    }
  }
  draw_sprite(dest, g_finished_dialog, FINISHED_X, FINISHED_Y);
}

/*=============================================================================
 * get_prop_text_width
 *============================================================================*/
int get_prop_text_width(char *text) {
  int width, offset;
  char *cur;

  cur = text;
  width = 0;
	while (*cur != 0) {
		offset = (*cur) - 32;    
		width += g_prop_font_width[offset] + 1;
    cur++;
	}  

  return width;
}

/*=============================================================================
 * render_centered_prop_text
 *============================================================================*/
void render_centered_prop_text(BITMAP *dest, char *text, int center, int y_pos) {
  int width;

  width = get_prop_text_width(text);
  render_prop_text(dest, text, center - (width/2), y_pos);
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

void update_overview_area(void) {
  int i, j;

  /* I and J represent a block of 4*4 pixels */
  clear_to_color(g_overview_box, 192);  
  for(i=0; i< g_picture->w / OVERVIEW_BLOCK_SIZE + 1; i++) {
    for(j=0; j< g_picture->h / OVERVIEW_BLOCK_SIZE + 1; j++) {
      update_overview_area_at(i, j);
    }
  }
}

void update_overview_area_at(int x, int y) {
  /* X and Y represent a block of 4*4 pixels */
  int start_x, start_y;
  int end_x, end_y;
  int i, j, complete_count, error_count;
  int correct_color, current_color;

  start_x = x * OVERVIEW_BLOCK_SIZE;
  start_y = y * OVERVIEW_BLOCK_SIZE;
  if(start_x > g_picture->w && start_y > g_picture->h) 
    return;

  if(start_x + OVERVIEW_BLOCK_SIZE > g_picture->w) 
    end_x = g_picture->w;
  else
    end_x = start_x + OVERVIEW_BLOCK_SIZE;
  if(start_y + OVERVIEW_BLOCK_SIZE > g_picture->h)
    end_y = g_picture->h;
  else
    end_y = start_y + OVERVIEW_BLOCK_SIZE;

  complete_count = 0;
  error_count = 0;
  for(j = start_y; j < end_y; j++) {
    for(i = start_x; i < end_x; i++) {
      correct_color=g_picture->pic_squares[j*g_picture->w + i].pal_entry;
      current_color=g_picture->pic_squares[j*g_picture->w + i].fill_value;
      if (current_color == correct_color)
        complete_count++;
      else if (current_color != correct_color && current_color != 0)
        error_count++;
    }
  }

  if(complete_count == 0)
    /* Draw blocks with nothing black */
    putpixel(g_overview_box, x, y, 208);
  else if(error_count > 0)
    /* Draw blocks with a mistake red */
    putpixel(g_overview_box, x, y, 198);    
  else if (complete_count > 0 && complete_count <
           OVERVIEW_BLOCK_SIZE * OVERVIEW_BLOCK_SIZE)
    /* Draw partial blocks light blue */
    putpixel(g_overview_box, x, y, 204);
  else
    /* Draw completed blocks white */
    putpixel(g_overview_box, x, y, 207);

}

/*=============================================================================
 * load_title
 *============================================================================*/
int load_title(void) {

  if(g_title_area == NULL) {
     g_title_area = create_bitmap(SCREEN_W, SCREEN_H);
     clear_to_color(g_title_area, 208);
  }
  g_title_box = (BITMAP *)g_res[RES_TITLEBOX].dat;
  return 0;
}

/*=============================================================================
 * load_logo
 *============================================================================*/
int load_logo(void) {
  g_logo = (BITMAP *)g_res[RES_HOLYGOAT].dat;
  return 0;
}

void free_graphics(void) {
  /* A couple graphics need to be deallocated before shutdown */
  if(g_overview_box != NULL)
    destroy_bitmap(g_overview_box);
  if(g_title_area != NULL)
    destroy_bitmap(g_title_area);
}

/*=============================================================================
 * load_graphics
 *============================================================================*/
int load_graphics(void) {

  g_numbers = (BITMAP *)g_res[RES_NUMBERS].dat;
  g_highlight_numbers = (BITMAP *)g_res[RES_NUMS_HI].dat;
  g_bg_lower = (BITMAP *)g_res[RES_BG_LOWER].dat;
  g_bg_right = (BITMAP *)g_res[RES_BG_RIGHT].dat;
  g_mainarea = (BITMAP *)g_res[RES_MAINAREA].dat;
  g_draw_cursor = (BITMAP *)g_res[RES_DRAWCURS].dat;
  g_small_pal = (BITMAP *)g_res[RES_SM_PAL].dat;
  g_large_pal = (BITMAP *)g_res[RES_LG_PAL].dat;
  g_large_diamonds = (BITMAP *)g_res[RES_LG_DIA].dat;
  g_large_crosses = (BITMAP *)g_res[RES_LG_CROSS].dat;
  g_pal_cursor = (BITMAP *)g_res[RES_PALCURS].dat;
  g_wrong = (BITMAP *)g_res[RES_WRONG].dat;
  g_page_buttons = (BITMAP *)g_res[RES_PAGEBUTN].dat;
  g_prop_font = (BITMAP *)g_res[RES_PROPFONT].dat;
  g_main_buttons = (BITMAP *)g_res[RES_BUTTONS].dat;
  g_save_notice = (BITMAP *)g_res[RES_SAVING].dat;
  g_load_notice = (BITMAP *)g_res[RES_LOADING].dat;
  g_load_dialog = (BITMAP *)g_res[RES_LOADDIAG].dat;
  g_finished_dialog = (BITMAP *)g_res[RES_FINISHED].dat;
  g_overview_box = create_bitmap(OVERVIEW_WIDTH, OVERVIEW_HEIGHT);
  g_overview_cursor = (BITMAP *)g_res[RES_OVERCURS].dat;
  g_mouse_cursor = (BITMAP *)g_res[RES_MOUSE].dat;
  g_help_previous = (BITMAP *)g_res[RES_HELP_PREVIOUS].dat;
  g_help_next = (BITMAP *)g_res[RES_HELP_NEXT].dat;
  g_help_exit = (BITMAP *)g_res[RES_HELP_EXIT].dat;

  /* We only want to create this once, so we check for null before we 
     create it */
  g_title_area = NULL;

  return 0;

}
