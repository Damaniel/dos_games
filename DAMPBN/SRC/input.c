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
#include "../include/globals.h"

unsigned char g_keypress_lockout[128];

/*=============================================================================
 * process_input
 *============================================================================*/
void process_input(int state) {
    if (state == STATE_GAME) {
      input_state_game();
    }
}

/*=============================================================================
 * input_state_game
 *============================================================================*/
void input_state_game(void) {
    ColorSquare c;

    /*-------------------------------------------------------------------------
     * ESC - exit the game
     *------------------------------------------------------------------------*/
    if(key[KEY_ESC]) {
      /* If the key was previously up... */
      if (!g_keypress_lockout[KEY_ESC]) {
          g_game_done = 1;
          g_keypress_lockout[KEY_ESC] = 1;
      }
    } 
    if(!key[KEY_ESC] && g_keypress_lockout[KEY_ESC]) {
      g_keypress_lockout[KEY_ESC] = 0;
    }
    
    /*-------------------------------------------------------------------------
     * left - move the cursor left in the play area
     *------------------------------------------------------------------------*/
    if(key[KEY_LEFT]) {
      /* If the key was previously up... */
      if (!g_keypress_lockout[KEY_LEFT]) {
        clear_render_components(&g_components);
        g_old_draw_cursor_x = g_draw_cursor_x;
        g_old_draw_cursor_y = g_draw_cursor_y;        
        /* If the SHIFT key is held, move a page.  Don't move cursor */
        if (key_shifts & KB_SHIFT_FLAG) {
          g_pic_render_x -= PLAY_AREA_W;
          g_components.render_main_area_squares = 1;          
        } else {
          /* Move the cursor, move the page if on the edge */
          g_draw_cursor_x--;
          if(g_draw_cursor_x < 0) {
            g_draw_cursor_x = 0;
            g_pic_render_x--;
            g_components.render_main_area_squares = 1;            
          }
        }
        /* Check to make sure the visible area is fully in the picture */
        if (g_pic_render_x < 0) {
              g_pic_render_x = 0;
        }
        /* Calculate where we'll be drawing within the whole picture */
        g_draw_position_x = g_pic_render_x + g_draw_cursor_x;
        g_draw_position_y = g_pic_render_y + g_draw_cursor_y;
        
        g_components.render_draw_cursor = 1;
        g_update_screen = 1;
        g_keypress_lockout[KEY_LEFT] = 1;
      }
    }
    if(!key[KEY_LEFT] && g_keypress_lockout[KEY_LEFT]) {
      g_keypress_lockout[KEY_LEFT] = 0;
    }

    /*-------------------------------------------------------------------------
     * right - move the cursor right in the play area 
     *------------------------------------------------------------------------*/
    if (key[KEY_RIGHT]) {
      /* If the key was previously up... */
      if (!g_keypress_lockout[KEY_RIGHT]) {
        clear_render_components(&g_components);
        g_old_draw_cursor_x = g_draw_cursor_x;
        g_old_draw_cursor_y = g_draw_cursor_y;             
        /* If the SHIFT key is held, move a page. Don't move cursor */
        if (key_shifts & KB_SHIFT_FLAG) {
          g_pic_render_x += PLAY_AREA_W;
          g_components.render_main_area_squares = 1;            
        } else {
          /* Move the cursor, move the page if on the edge */
          g_draw_cursor_x++;
          if(g_draw_cursor_x >= PLAY_AREA_W) {
            g_draw_cursor_x = PLAY_AREA_W -1;
            g_pic_render_x++;
            g_components.render_main_area_squares = 1;              
          }
        }
        /* Check to make sure the visible area is fully in the picture */
        if (g_pic_render_x >= g_picture->w - PLAY_AREA_W) {
          g_pic_render_x = g_picture->w - PLAY_AREA_W;
        }
        /* Calculate where we'll be drawing within the whole picture */        
        g_draw_position_x = g_pic_render_x + g_draw_cursor_x;
        g_draw_position_y = g_pic_render_y + g_draw_cursor_y;

        g_components.render_draw_cursor = 1;
        g_update_screen = 1;
        g_keypress_lockout[KEY_RIGHT] = 1;
      }
    }
    if (!key[KEY_RIGHT] && g_keypress_lockout[KEY_RIGHT]) {
      g_keypress_lockout[KEY_RIGHT] = 0;
    }

    /*-------------------------------------------------------------------------
     * up - move the cursor up in the play area 
     *------------------------------------------------------------------------*/
    if (key[KEY_UP]) {
      /* If the key was previously up */
      if (!g_keypress_lockout[KEY_UP]) {
        clear_render_components(&g_components);
        g_old_draw_cursor_x = g_draw_cursor_x;
        g_old_draw_cursor_y = g_draw_cursor_y;              
        /* If the SHIFT key is held, move a page.  Don't move cursor */
        if (key_shifts & KB_SHIFT_FLAG) {
          g_pic_render_y -= PLAY_AREA_H;
          g_components.render_main_area_squares = 1;              
        } else {
          /* Move the cursor, move the page if on the edge */
          g_draw_cursor_y--;
          if(g_draw_cursor_y <0) {
            g_draw_cursor_y = 0;
            g_pic_render_y--;
            g_components.render_main_area_squares = 1;                
          }
        }
        /* Check to make sure the visible area is fully in the picture */
        if (g_pic_render_y < 0) {
          g_pic_render_y = 0;
        }
        /* Calculate where we'll be drawing within the whole picture */
        g_draw_position_x = g_pic_render_x + g_draw_cursor_x;
        g_draw_position_y = g_pic_render_y + g_draw_cursor_y;

        g_components.render_draw_cursor = 1;
        g_update_screen = 1;
        g_keypress_lockout[KEY_UP] = 1;
      }
    }
    if (!key[KEY_UP] && g_keypress_lockout[KEY_UP]) {
      g_keypress_lockout[KEY_UP] = 0;
    }

    /*-------------------------------------------------------------------------
     * down - move the cursor down in the play area 
     *------------------------------------------------------------------------*/
    if (key[KEY_DOWN]) {
      /* If the key was previously up */
      if (!g_keypress_lockout[KEY_DOWN]) {
        clear_render_components(&g_components);
        g_old_draw_cursor_x = g_draw_cursor_x;
        g_old_draw_cursor_y = g_draw_cursor_y;
        
        /* If the SHIFT key is held, move a page.  Don't move cursor */
        if (key_shifts & KB_SHIFT_FLAG) {
          g_pic_render_y += PLAY_AREA_H;
          g_components.render_main_area_squares = 1;
        } else {
          g_draw_cursor_y++;
          if(g_draw_cursor_y >= PLAY_AREA_H) {
            g_draw_cursor_y = PLAY_AREA_H -1;
            g_pic_render_y++;
            g_components.render_main_area_squares = 1;
          }
        }
        /* Check to make sure the visible area is fully in the picture */
        if (g_pic_render_y >= g_picture->h - PLAY_AREA_H) {
          g_pic_render_y = g_picture->h - PLAY_AREA_H;
        }
        /* Calculate where we'll be drawing within the whole picture */        
        g_draw_position_x = g_pic_render_x + g_draw_cursor_x;
        g_draw_position_y = g_pic_render_y + g_draw_cursor_y;

        g_components.render_draw_cursor = 1;
        g_update_screen = 1;
        g_keypress_lockout[KEY_DOWN] = 1;
      }
    }
    if (!key[KEY_DOWN] && g_keypress_lockout[KEY_DOWN]) {
      g_keypress_lockout[KEY_DOWN] = 0;
    }

    /*-------------------------------------------------------------------------
     * P - toggle palette page
     *------------------------------------------------------------------------*/
    if (key[KEY_P]) {
      if (!g_keypress_lockout[KEY_P]) {
        g_prev_color = g_cur_color;        
        if (g_palette_page == 0) {
          /* Only change if there's more than one page worth of colors */
          if (g_picture->num_colors > PALETTE_COLORS_PER_PAGE) {
            g_palette_page = 1;
            g_cur_color += PALETTE_COLORS_PER_PAGE;
            /* Change previous color to represent the index on the new page */
            g_prev_color += PALETTE_COLORS_PER_PAGE;
          }
        }
        else {
          g_palette_page = 0;
          g_cur_color -= PALETTE_COLORS_PER_PAGE;
          /* Change previous color to represent the index on the new page */
          g_prev_color -= PALETTE_COLORS_PER_PAGE;          
        }
        /* If the wrapped color is now higher than the biggest color index,
           clamp it. */
        if (g_cur_color > g_picture->num_colors)
          g_cur_color = g_picture->num_colors;

        clear_render_components(&g_components);
        g_components.render_palette_area = 1;
        g_components.render_palette_cursor = 1;
        g_update_screen = 1;
        g_keypress_lockout[KEY_P] = 1;
      }
    }
    if(!key[KEY_P] && g_keypress_lockout[KEY_P]) {
      g_keypress_lockout[KEY_P] = 0;
    }     

    /*-------------------------------------------------------------------------
     * Open Brace ([) - move to previous palette color
     *------------------------------------------------------------------------*/
    if (key[KEY_OPENBRACE]) {
      if (!g_keypress_lockout[KEY_OPENBRACE]) {
        /* Change to the previous color index */
        g_prev_color = g_cur_color;
        g_cur_color--;
        /* If at the lowest index on the page, wrap to the highest */
        if (g_palette_page == 0) {
          if (g_cur_color < FIRST_COLOR_ON_FIRST_PAGE)
            g_cur_color = LAST_COLOR_ON_FIRST_PAGE;      
        } 
        if (g_palette_page == 1) {
          if (g_cur_color < FIRST_COLOR_ON_SECOND_PAGE)
            g_cur_color = LAST_COLOR_ON_SECOND_PAGE;
        }
        /* If the wrapped color is now higher than the biggest color index,
           clamp it. */
        if (g_cur_color > g_picture->num_colors)
          g_cur_color = g_picture->num_colors;

        clear_render_components(&g_components);
        g_components.render_palette_cursor = 1;
        g_update_screen = 1;
        g_keypress_lockout[KEY_OPENBRACE] = 1;
      }
    }
    if(!key[KEY_OPENBRACE] && g_keypress_lockout[KEY_OPENBRACE]) {
      g_keypress_lockout[KEY_OPENBRACE] = 0;
    }

    /*-------------------------------------------------------------------------
     * Open Brace ([) - move to next palette color
     *------------------------------------------------------------------------*/
    if (key[KEY_CLOSEBRACE]) {
      if (!g_keypress_lockout[KEY_CLOSEBRACE]) {
        /* Change to the next color index */
        g_prev_color = g_cur_color;
        g_cur_color++;
        /* If we reach the end of the palette on the current page,
           either because we hit the last index of the page, or the end of the
           picture's palette, wrap back around. */
        if (g_palette_page == 0) {
          if (g_cur_color > LAST_COLOR_ON_FIRST_PAGE || 
              g_cur_color > g_picture->num_colors)
            g_cur_color = FIRST_COLOR_ON_FIRST_PAGE;
        } 
        if (g_palette_page == 1) {
          if (g_cur_color > LAST_COLOR_ON_SECOND_PAGE || 
              g_cur_color > g_picture->num_colors)
            g_cur_color = FIRST_COLOR_ON_SECOND_PAGE;
        }

        clear_render_components(&g_components);
        g_components.render_palette_cursor = 1;
        g_update_screen = 1;
        g_keypress_lockout[KEY_CLOSEBRACE] = 1;
      }
    }
    if(!key[KEY_CLOSEBRACE] && g_keypress_lockout[KEY_CLOSEBRACE]) {
      g_keypress_lockout[KEY_CLOSEBRACE] = 0;
    }

    /*-------------------------------------------------------------------------
     * Space - Mark the highlighted square with the current color
     *------------------------------------------------------------------------*/
     if (key[KEY_SPACE]) {
       if (!g_keypress_lockout[KEY_SPACE]) {
        /* If unfilled, fill with the active color.  If filled, unfill it. */
        if(g_picture->pic_squares[(g_draw_position_y) * g_picture->w +
                               (g_draw_position_x)].fill_value != 0) {
           g_picture->pic_squares[(g_draw_position_y) * g_picture->w +
                               (g_draw_position_x)].fill_value = 0;
        } else {
           g_picture->pic_squares[(g_draw_position_y) * g_picture->w +
                               (g_draw_position_x)].fill_value = g_cur_color;
        }
        clear_render_components(&g_components);
        g_components.render_draw_cursor = 1;
        g_update_screen = 1;
        g_keypress_lockout[KEY_SPACE] = 1;
       }
     }
     if(!key[KEY_SPACE] && g_keypress_lockout[KEY_SPACE]) {
      g_keypress_lockout[KEY_SPACE] = 0;
     }

}
