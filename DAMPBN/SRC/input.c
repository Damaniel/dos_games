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

int process_input(int state) {

    int update;

    update = 0;

    /*----------------------
     * ESC
     *----------------------*/
    if(key[KEY_ESC]) {
      if (!g_keypress_lockout[KEY_ESC]) {
          g_game_done = 1;
          g_keypress_lockout[KEY_ESC] = 1;
      }
    } 
    if(!key[KEY_ESC] && g_keypress_lockout[KEY_ESC]) {
      g_keypress_lockout[KEY_ESC] = 0;
    }
    
    /*-----------------------
     * left
     *-----------------------*/
    if(key[KEY_LEFT]) {
      /* If the key was previously up... */
      if (!g_keypress_lockout[KEY_LEFT]) {
        /* If the SHIFT key is held, move a page.  Don't move cursor */
        if (key_shifts & KB_SHIFT_FLAG) {
          g_pic_render_x -= PLAY_AREA_W;
        } else {
          /* Move the cursor, move the page if on the edge */
          g_draw_cursor_x--;
          if(g_draw_cursor_x < 0) {
            g_draw_cursor_x = 0;
            g_pic_render_x--;
          }
        }
        /* Check to make sure the visible area is fully in the picture */
        if (g_pic_render_x < 0) {
              g_pic_render_x = 0;
        }
        g_draw_position_x = g_pic_render_x + g_draw_cursor_x;
        g_draw_position_y = g_pic_render_y + g_draw_cursor_y;
        clear_render_components(&g_components);
        g_components.render_main_area_squares = 1;
        g_components.render_draw_cursor = 1;
        update = 1;
        g_keypress_lockout[KEY_LEFT] = 1;
      }
    }
    if(!key[KEY_LEFT] && g_keypress_lockout[KEY_LEFT]) {
      g_keypress_lockout[KEY_LEFT] = 0;
    }

    /*-----------------------
     * right
     *-----------------------*/
    if (key[KEY_RIGHT]) {
      /* If the key was previously up... */
      if (!g_keypress_lockout[KEY_RIGHT]) {
        /* If the SHIFT key is held, move a page. Don't move cursor */
        if (key_shifts & KB_SHIFT_FLAG) {
          g_pic_render_x += PLAY_AREA_W;
        } else {
          /* Move the cursor, move the page if on the edge */
          g_draw_cursor_x++;
          if(g_draw_cursor_x >= PLAY_AREA_W) {
            g_draw_cursor_x = PLAY_AREA_W -1;
            g_pic_render_x++;
          }
        }
        /* Check to make sure the visible area is fully in the picture */
        if (g_pic_render_x >= g_picture->w - PLAY_AREA_W) {
          g_pic_render_x = g_picture->w - PLAY_AREA_W;
        }
        g_draw_position_x = g_pic_render_x + g_draw_cursor_x;
        g_draw_position_y = g_pic_render_y + g_draw_cursor_y;
        clear_render_components(&g_components);
        g_components.render_main_area_squares = 1;
        g_components.render_draw_cursor = 1;
        update = 1;
        g_keypress_lockout[KEY_RIGHT] = 1;
      }
    }
    if (!key[KEY_RIGHT] && g_keypress_lockout[KEY_RIGHT]) {
      g_keypress_lockout[KEY_RIGHT] = 0;
    }

    /*-----------------------
     * up
     *-----------------------*/
    if (key[KEY_UP]) {
      /* If the key was previously up */
      if (!g_keypress_lockout[KEY_UP]) {
        /* If the SHIFT key is held, move a page.  Don't move cursor */
        if (key_shifts & KB_SHIFT_FLAG) {
          g_pic_render_y -= PLAY_AREA_H;
        } else {
          /* Move the cursor, move the page if on the edge */
          g_draw_cursor_y--;
          if(g_draw_cursor_y <0) {
            g_draw_cursor_y = 0;
            g_pic_render_y--;
          }
        }
        /* Check to make sure the visible area is fully in the picture */
        if (g_pic_render_y < 0) {
          g_pic_render_y = 0;
        }
        g_draw_position_x = g_pic_render_x + g_draw_cursor_x;
        g_draw_position_y = g_pic_render_y + g_draw_cursor_y;
        clear_render_components(&g_components);
        g_components.render_main_area_squares = 1;
        g_components.render_draw_cursor = 1;
        update = 1;
        g_keypress_lockout[KEY_UP] = 1;
      }
    }
    if (!key[KEY_UP] && g_keypress_lockout[KEY_UP]) {
      g_keypress_lockout[KEY_UP] = 0;
    }

    /*-----------------------
     * down
     *-----------------------*/
    if (key[KEY_DOWN]) {
      /* If the key was previously up */
      if (!g_keypress_lockout[KEY_DOWN]) {
        /* If the SHIFT key is held, move a page.  Don't move cursor */
        if (key_shifts & KB_SHIFT_FLAG) {
          g_pic_render_y += PLAY_AREA_H;
        } else {
          g_draw_cursor_y++;
          if(g_draw_cursor_y >= PLAY_AREA_H) {
            g_draw_cursor_y = PLAY_AREA_H -1;
            g_pic_render_y++;
          }
        }
        /* Check to make sure the visible area is fully in the picture */
        if (g_pic_render_y >= g_picture->h - PLAY_AREA_H) {
          g_pic_render_y = g_picture->h - PLAY_AREA_H;
        }
        g_draw_position_x = g_pic_render_x + g_draw_cursor_x;
        g_draw_position_y = g_pic_render_y + g_draw_cursor_y;
        clear_render_components(&g_components);
        g_components.render_main_area_squares = 1;
        g_components.render_draw_cursor = 1;
        update = 1;
        g_keypress_lockout[KEY_DOWN] = 1;
      }
    }
    if (!key[KEY_DOWN] && g_keypress_lockout[KEY_DOWN]) {
      g_keypress_lockout[KEY_DOWN] = 0;
    }

    /*-----------------------
     * P
     *-----------------------*/
    if (key[KEY_P]) {
      if (!g_keypress_lockout[KEY_P]) {
        if (g_palette_page == 0) {
          g_palette_page = 1;
        }
        else {
          g_palette_page = 0;
        }
        clear_render_components(&g_components);
        g_components.render_palette_area = 1;
        g_components.render_palette = 1;
        update = 1;
        g_keypress_lockout[KEY_P] = 1;
      }
    }
    if(!key[KEY_P] && g_keypress_lockout[KEY_P]) {
      g_keypress_lockout[KEY_P] = 0;
    }     

    return update;
}