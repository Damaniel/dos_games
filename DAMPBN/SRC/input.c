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
#include <string.h>
#include "../include/globals.h"

unsigned char g_keypress_lockout[128];
int g_mouse_click_lockout;

int g_highlight_style_button; 
int g_highlight_save_button;
int g_highlight_load_button;

int g_mouse_x;
int g_mouse_y;
int g_old_mouse_x;
int g_old_mouse_y;
int g_game_area_mouse_mode;

int g_keyboard_has_priority;

/*=============================================================================
 * process_input
 *============================================================================*/
void process_input(int state) {

  update_mouse_status();

  switch(state) {
    case STATE_LOGO:
      input_state_logo();
      break;
    case STATE_TITLE:
      input_state_title();
      break;
    case STATE_GAME:
      input_state_game();
      break;
    case STATE_MAP:
      input_state_map();
      break;
    case STATE_SAVE:
      /* No keypresses are supported here */
      break;
    case STATE_LOAD:
      /* No keypresses are supported here */
      break;
    case STATE_HELP:
      input_state_help();
      break;
    case STATE_OPTS:
      break;
    case STATE_LOAD_DIALOG:
      input_state_load_dialog();
      break;
    case STATE_FINISHED:
      /* No keypresses are supported here */
      break;
    case STATE_REPLAY:
      input_state_replay();
    default:
      break;
  }
}

/*=============================================================================
 * is_in_game_area
 *============================================================================*/
int is_in_game_area(int x, int y) {
  int in_area = 0;
  int dx, dy, dw, dh;

  dx = DRAW_AREA_X;
  dy = DRAW_AREA_Y;
  dw =  DRAW_AREA_X + DRAW_AREA_WIDTH;
  dh = DRAW_AREA_Y + DRAW_AREA_HEIGHT;

  if(g_picture->w < MAX_PLAY_AREA_WIDTH) {
    dx = 0;
    dw = g_picture->w * NUMBER_BOX_RENDER_X_OFFSET;
  }
  if (g_picture->h < MAX_PLAY_AREA_HEIGHT) {
    dy = 0;
    dh = g_picture->h * NUMBER_BOX_RENDER_Y_OFFSET;
  }

  if(x >= dx && x < dw &&
     y >= dy && y < dh) {
       in_area = 1;
  }
  return in_area;
}

/*=============================================================================
 * is_in_overview_area
 *============================================================================*/
int is_in_overview_area(int x, int y) {
  int in_area = 0;

  if(x >= OVERVIEW_X && x < OVERVIEW_X + OVERVIEW_WIDTH &&
    y >= OVERVIEW_Y && y < OVERVIEW_Y + OVERVIEW_HEIGHT) {
      in_area = 1;
  }

  return in_area;
}

/*=============================================================================
 * update_mouse_status
 *============================================================================*/
void update_mouse_status(void) {
  /* If the mouse press is locked out and the button has been released,
   * remove the lockout */

  if (g_mouse_click_lockout) {
    if(!(mouse_b & 1)) {
      g_mouse_click_lockout = 0;
      /* Any buttons that should unhighlight when released do so hwere. */
      g_highlight_style_button = 0;
      g_highlight_load_button = 0;
      g_highlight_save_button = 0;
      /* Update the buttons on the next screen update */
      g_components.render_buttons = 1;
    }
  }
  else {
    if(!(mouse_b & 1)) {
      /* If the mouse is released and we're in draw mode or erase mode,
         go back to neutral */
      if (g_game_area_mouse_mode == MOUSE_MODE_DRAW) {
        g_game_area_mouse_mode = MOUSE_MODE_NEUTRAL;
      }
      if (g_game_area_mouse_mode == MOUSE_MODE_ERASE) {
        g_game_area_mouse_mode = MOUSE_MODE_NEUTRAL;
      }
    }
  }
}

/*=============================================================================
 * mouse_clicked_here
 *============================================================================*/
int mouse_clicked_here(int x1, int y1, int x2, int y2, int lockout) {
  int clicked_here = 0;

  /* Don't process complete mouse clicks outside of neutral mode */
  if (g_game_area_mouse_mode != MOUSE_MODE_NEUTRAL) {
    return 0;
  }

  /* If the mouse button is pressed, check to see if we're in the specified
     region.  If so, return 1.  If lockout is non-zero, 
     lock out further clicks until the button is released. */
  if ((mouse_b & 1) && mouse_x >= x1 && mouse_x < x2 && mouse_y >= y1 && mouse_y < y2) {
    if (g_mouse_click_lockout == 0) {
      g_mouse_click_lockout = lockout;  
      clicked_here = 1;
    }
  }

  return clicked_here;
}

/*=============================================================================
 * get_square_at
 *============================================================================*/
Position get_square_at(int x, int y) {
    Position p;

    p.x = (x - DRAW_AREA_X) / NUMBER_BOX_RENDER_X_OFFSET;
    p.y = (y - DRAW_AREA_Y) / NUMBER_BOX_RENDER_Y_OFFSET;

    return p;
}

/*=============================================================================
 * process_palette_press
 *============================================================================*/
void process_palette_press(void) {
  int process_page_change = 0;

  /* Process any mouse clicks on either of the palette buttons to change pages */
  if (mouse_clicked_here(PAGE_1_BUTTON_X, 
                         PAGE_1_BUTTON_Y,
                         PAGE_1_BUTTON_X + PAGE_BUTTON_WIDTH,
                         PAGE_1_BUTTON_Y + PAGE_BUTTON_HEIGHT,
                         1) && g_palette_page == 1) {
    g_palette_page = 0;
    g_cur_color -= PALETTE_COLORS_PER_PAGE;
    /* Change previous color to represent the index on the new page */
    g_prev_color -= PALETTE_COLORS_PER_PAGE;     
    process_page_change = 1;
  }
 
   if (mouse_clicked_here(PAGE_2_BUTTON_X, 
                         PAGE_2_BUTTON_Y,
                         PAGE_2_BUTTON_X + PAGE_BUTTON_WIDTH,
                         PAGE_2_BUTTON_Y + PAGE_BUTTON_HEIGHT,
                         1) && g_palette_page == 0) {
    /* Only change if there's more than one page worth of colors */
    if (g_picture->num_colors > PALETTE_COLORS_PER_PAGE) {
      g_palette_page = 1;
      g_cur_color += PALETTE_COLORS_PER_PAGE;
      /* Change previous color to represent the index on the new page */
      g_prev_color += PALETTE_COLORS_PER_PAGE;
      process_page_change = 1;
    }
  }
 
  /* Process the P key to toggle between pages */
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
      process_page_change = 1;
      g_keypress_lockout[KEY_P] = 1;
    }
  }
  if(!key[KEY_P] && g_keypress_lockout[KEY_P]) {
    g_keypress_lockout[KEY_P] = 0;
  }     

  if (process_page_change) {
    /* If the wrapped color is now higher than the biggest color index,
       clamp it. */
    if (g_cur_color > g_picture->num_colors)
      g_cur_color = g_picture->num_colors;
     clear_render_components(&g_components);
    /* If marking is turned on, redraw the play area to update the
       highlights */
    if (g_mark_current == 1 ) {
      g_components.render_main_area_squares = 1;        
      g_components.render_draw_cursor = 1;
      }  
    g_components.render_palette_area = 1;
    g_components.render_palette_cursor = 1;
  }
}

/*=============================================================================
 * process_mark_press
 *============================================================================*/
void process_mark_press(void) {
  int process_mark = 0;

  /* Process any mouse clicks on the Mark button */
  if (mouse_clicked_here(MARK_BUTTON_X , 
                         MARK_BUTTON_Y,
                         MARK_BUTTON_X  + MENU_BUTTON_WIDTH,
                         MARK_BUTTON_Y  + MENU_BUTTON_HEIGHT,
                         1)) {
    process_mark = 1;
  }

  /*-------------------------------------------------------------------------
   * K - toggle highlighting of active color in the play area
   *------------------------------------------------------------------------*/     
  if (key[KEY_K]) {
    if (!g_keypress_lockout[KEY_K]) {
      process_mark = 1;
    }
  }
  if (!key[KEY_K] && g_keypress_lockout[KEY_K]) {
    g_keypress_lockout[KEY_K] = 0;
  }

  if (process_mark) {
    if (g_mark_current == 0) {
      g_mark_current = 1;
    }
    else {
      g_mark_current = 0;      
    }
    clear_render_components(&g_components);
    g_components.render_main_area_squares = 1;
    g_components.render_draw_cursor = 1;
    g_components.render_buttons = 1;
    g_keypress_lockout[KEY_K] = 1;
  }      
}

/*=============================================================================
 * process_palette_color_press
 *============================================================================*/
void process_palette_color_press(void) {

  int i;
  int entry = 1;
  int change_color = 0;
  int update_display = 0;

  /*-------------------------------------------------------------------------
   * check for mouse clicks in palette area
   *------------------------------------------------------------------------*/
  for (i=0; i<NUM_PALETTE_ROWS; i++) {
    if (mouse_clicked_here(PALETTE_COLUMN_1_X, 
                           PALETTE_COLUMN_Y +  PALETTE_ENTRY_HEIGHT * i,
                           PALETTE_COLUMN_1_X + PALETTE_ENTRY_WIDTH,
                           PALETTE_COLUMN_Y +  PALETTE_ENTRY_HEIGHT * (i+1),
                           1)) {
                             g_prev_color = g_cur_color;
                             entry = i;
                             change_color = 1;
                           }
    if (mouse_clicked_here(PALETTE_COLUMN_2_X, 
                           PALETTE_COLUMN_Y +  PALETTE_ENTRY_HEIGHT * i,
                           PALETTE_COLUMN_2_X + PALETTE_ENTRY_WIDTH,
                           PALETTE_COLUMN_Y +  PALETTE_ENTRY_HEIGHT * (i+1),
                           1)) {
                             g_prev_color = g_cur_color;
                             entry = NUM_PALETTE_ROWS + i;
                             change_color = 1;
                           }
    if (mouse_clicked_here(PALETTE_COLUMN_3_X, 
                           PALETTE_COLUMN_Y +  PALETTE_ENTRY_HEIGHT * i,
                           PALETTE_COLUMN_3_X + PALETTE_ENTRY_WIDTH,
                           PALETTE_COLUMN_Y +  PALETTE_ENTRY_HEIGHT * (i+1),
                           1)) {
                             g_prev_color = g_cur_color;
                             entry = (2 * NUM_PALETTE_ROWS) + i;
                             change_color = 1;
                           }    
    if (mouse_clicked_here(PALETTE_COLUMN_4_X, 
                           PALETTE_COLUMN_Y +  PALETTE_ENTRY_HEIGHT * i,
                           PALETTE_COLUMN_4_X + PALETTE_ENTRY_WIDTH,
                           PALETTE_COLUMN_Y +  PALETTE_ENTRY_HEIGHT * (i+1),
                           1)) {
                             g_prev_color = g_cur_color;
                             entry = (3 * NUM_PALETTE_ROWS) + i;
                             change_color = 1;
                           }    

    if (change_color) {
      if (g_palette_page == 0) {
        if (entry < g_picture->num_colors) {
          g_cur_color = entry + 1;
          update_display = 1;
        }
      } else {
        if (entry + PALETTE_COLORS_PER_PAGE < g_picture->num_colors) {
          g_cur_color = PALETTE_COLORS_PER_PAGE + entry + 1;
          update_display = 1;
        }
      }
    }                           
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
       
      /* Update relevant screen components */
      update_display = 1;

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

      /* Update relevant screen components */
      update_display = 1;

      g_keypress_lockout[KEY_CLOSEBRACE] = 1;
    }
  }
  if(!key[KEY_CLOSEBRACE] && g_keypress_lockout[KEY_CLOSEBRACE]) {
    g_keypress_lockout[KEY_CLOSEBRACE] = 0;
  }

  if (update_display) {
     clear_render_components(&g_components);
    /* If marking is turned on, redraw the play area to update the
       highlights */        
    if (g_mark_current == 1 ) {
      g_components.render_main_area_squares = 1;        
      g_components.render_draw_cursor = 1;
    }                  
    g_components.render_palette_cursor = 1;  
  }
}

/*=============================================================================
 * process_help_press
 *============================================================================*/
void process_help_press(void) {

  /* Check to see if the help button was clicked */
  if (mouse_clicked_here(HELP_BUTTON_X , 
                         HELP_BUTTON_Y,
                         HELP_BUTTON_X  + MENU_BUTTON_WIDTH,
                         HELP_BUTTON_Y  + MENU_BUTTON_HEIGHT,
                         1)) {
    change_state(STATE_HELP, STATE_GAME);
  }

  /*-------------------------------------------------------------------------
   * H - display help
   *------------------------------------------------------------------------*/ 
  if (key[KEY_H]) {
    if (!g_keypress_lockout[KEY_H]) {    
      change_state(STATE_HELP, STATE_GAME);
      g_keypress_lockout[KEY_H] = 1;        
    }
  }
  if (!key[KEY_H] && g_keypress_lockout[KEY_H]) {
    g_keypress_lockout[KEY_H] = 0;
  }      
}

 /*=============================================================================
 * process_map_press
 *============================================================================*/
void process_map_press(void) {

  /* Check to see if the exit button was clicked */
  if (mouse_clicked_here(MAP_BUTTON_X , 
                         MAP_BUTTON_Y,
                         MAP_BUTTON_X  + MENU_BUTTON_WIDTH,
                         MAP_BUTTON_Y  + MENU_BUTTON_HEIGHT,
                         1)) {
    change_state(STATE_MAP, STATE_GAME);
  }

  /*-------------------------------------------------------------------------
   * M - display progress map
   *------------------------------------------------------------------------*/ 
  if (key[KEY_M]) {
    if (!g_keypress_lockout[KEY_M]) {    
      change_state(STATE_MAP, STATE_GAME);
      g_keypress_lockout[KEY_M] = 1;        
    }
  }
  if (!key[KEY_M] && g_keypress_lockout[KEY_M]) {
    g_keypress_lockout[KEY_M] = 0;
  }    
}

/*=============================================================================
 * process_exit_press
 *============================================================================*/
void process_exit_press(void) {

  /* Check to see if the exit button was clicked */
  if (mouse_clicked_here(EXIT_BUTTON_X , 
                         EXIT_BUTTON_Y,
                         EXIT_BUTTON_X  + MENU_BUTTON_WIDTH,
                         EXIT_BUTTON_Y  + MENU_BUTTON_HEIGHT,
                         1)) {
    change_state(STATE_TITLE, STATE_GAME);
  }
  
  /*-------------------------------------------------------------------------
   * ESC - exit the game
   *------------------------------------------------------------------------*/
  if(key[KEY_ESC]) {
    /* If the key was previously up... */
    if (!g_keypress_lockout[KEY_ESC]) {
        g_keypress_lockout[KEY_ESC] = 1;
        change_state(STATE_TITLE, STATE_GAME);
    }
  } 
  if(!key[KEY_ESC] && g_keypress_lockout[KEY_ESC]) {
    g_keypress_lockout[KEY_ESC] = 0;
  }  
}

/*=============================================================================
 * process_style_press
 *============================================================================*/
void process_style_press(void) {
  int update = 0;

  /* Check to see if the style button was clicked */
  if (mouse_clicked_here(STYLE_BUTTON_X , 
                         STYLE_BUTTON_Y,
                         STYLE_BUTTON_X  + MENU_BUTTON_WIDTH,
                         STYLE_BUTTON_Y  + MENU_BUTTON_HEIGHT,
                         1)) {
    update = 1;
  }
    /*-------------------------------------------------------------------------
     * T - toggle through tile types
     *------------------------------------------------------------------------*/ 
  if (key[KEY_T]) {
    if(!g_keypress_lockout[KEY_T]) {
      update = 1;        
      g_keypress_lockout[KEY_T] = 1;      
    }
  }
  if(!key[KEY_T] && g_keypress_lockout[KEY_T]) {
    g_keypress_lockout[KEY_T] = 0;
  }

  if(update) {
    g_draw_style++;
    if (g_draw_style >= NUM_STYLES) 
      g_draw_style = 0;    
    clear_render_components(&g_components);
    g_components.render_main_area_squares = 1;
    g_components.render_draw_cursor = 1;
    g_components.render_buttons = 1;
    g_highlight_style_button = 1;
  }
}

/*=============================================================================
 * process_save_press
 *============================================================================*/
void process_save_press(void) {

  /* Check to see if the save button was clicked */
  if (mouse_clicked_here(SAVE_BUTTON_X , 
                         SAVE_BUTTON_Y,
                         SAVE_BUTTON_X  + MENU_BUTTON_WIDTH,
                         SAVE_BUTTON_Y  + MENU_BUTTON_HEIGHT,
                         1)) {
    g_highlight_save_button = 1;       
    g_components.render_buttons = 1;                        
    change_state(STATE_SAVE, STATE_GAME);
  }

/*-------------------------------------------------------------------------
   * S - save a progress file
   *------------------------------------------------------------------------*/ 
  if (key[KEY_S]) {
    if(!g_keypress_lockout[KEY_S]) {
      g_highlight_save_button = 1;       
      g_components.render_buttons = 1;          
      g_keypress_lockout[KEY_S] = 1;           
      change_state(STATE_SAVE, STATE_GAME); 
    }
  }
  if(!key[KEY_S] && g_keypress_lockout[KEY_S]) {
    g_keypress_lockout[KEY_S] = 0;
  }    
}

/*=============================================================================
 * process_load_press
 *============================================================================*/
void process_load_press(void) {
  /* Check to see if the save button was clicked */
  if (mouse_clicked_here(LOAD_BUTTON_X , 
                         LOAD_BUTTON_Y,
                         LOAD_BUTTON_X  + MENU_BUTTON_WIDTH,
                         LOAD_BUTTON_Y  + MENU_BUTTON_HEIGHT,
                         1)) {
    g_highlight_load_button = 1;       
    g_components.render_buttons = 1;                               
    change_state(STATE_LOAD_DIALOG, STATE_GAME);
  }

  /*-------------------------------------------------------------------------
   * L - load a progress file
   *------------------------------------------------------------------------*/ 
  if (key[KEY_L]) {
    if(!g_keypress_lockout[KEY_L]) {
        g_highlight_load_button = 1;       
        g_components.render_buttons = 1;                     
        g_keypress_lockout[KEY_L] = 1;      
        change_state(STATE_LOAD_DIALOG, STATE_GAME);
    }
  }
  if(!key[KEY_L] && g_keypress_lockout[KEY_L]) {
    g_keypress_lockout[KEY_L] = 0;
  }    
}

/*=============================================================================
 * process_main_area_keyboard_input
 *============================================================================*/
void process_main_area_keyboard_input(void) {
  int square_offset, fill_val, pal_val;
  int moved, done;

  moved = 0;
  done = 0;

  /*-------------------------------------------------------------------------
   * left - move the cursor left in the play area
   *------------------------------------------------------------------------*/
  if (key[KEY_LEFT]) {
    /* If the key was previously up... */
    if (!g_keypress_lockout[KEY_LEFT] && moved == 0) {
      moved = 1;
      clear_render_components(&g_components);
      g_old_draw_cursor_x = g_draw_cursor_x;
      g_old_draw_cursor_y = g_draw_cursor_y;        
      /* If the SHIFT key is held, move a page.  Don't move cursor */
      if (key_shifts & KB_SHIFT_FLAG) {
        g_pic_render_x -= g_play_area_w;
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
      g_components.render_scrollbars = 1;
      g_components.render_overview_display = 1;        
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
    if (!g_keypress_lockout[KEY_RIGHT] && moved == 0) {
      moved = 1;
      clear_render_components(&g_components);
      g_old_draw_cursor_x = g_draw_cursor_x;
      g_old_draw_cursor_y = g_draw_cursor_y;             
      /* If the SHIFT key is held, move a page. Don't move cursor */
      if (key_shifts & KB_SHIFT_FLAG) {
        g_pic_render_x += g_play_area_w;
        g_components.render_main_area_squares = 1;            
      } else {
        /* Move the cursor, move the page if on the edge */
        g_draw_cursor_x++;
        if(g_draw_cursor_x >= g_play_area_w) {
          g_draw_cursor_x = g_play_area_w - 1;
          g_pic_render_x++;
          g_components.render_main_area_squares = 1;              
        }
      }
      /* Check to make sure the visible area is fully in the picture */
      if (g_pic_render_x >= g_picture->w - g_play_area_w) {
        g_pic_render_x = g_picture->w - g_play_area_w;
      }
      /* Calculate where we'll be drawing within the whole picture */        
      g_draw_position_x = g_pic_render_x + g_draw_cursor_x;
      g_draw_position_y = g_pic_render_y + g_draw_cursor_y;

      g_components.render_draw_cursor = 1;
      g_components.render_scrollbars = 1;
      g_components.render_overview_display = 1;        
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
    if (!g_keypress_lockout[KEY_UP] && moved == 0) {
      moved = 1;
      clear_render_components(&g_components);
      g_old_draw_cursor_x = g_draw_cursor_x;
      g_old_draw_cursor_y = g_draw_cursor_y;              
      /* If the SHIFT key is held, move a page.  Don't move cursor */
      if (key_shifts & KB_SHIFT_FLAG) {
        g_pic_render_y -= g_play_area_h;
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
      g_components.render_scrollbars = 1;
      g_components.render_overview_display = 1;        
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
    if (!g_keypress_lockout[KEY_DOWN] && moved == 0) {
      moved = 1;
      clear_render_components(&g_components);
      g_old_draw_cursor_x = g_draw_cursor_x;
      g_old_draw_cursor_y = g_draw_cursor_y;
        
      /* If the SHIFT key is held, move a page.  Don't move cursor */
      if (key_shifts & KB_SHIFT_FLAG) {
        g_pic_render_y += g_play_area_h;
        g_components.render_main_area_squares = 1;
      } else {
        g_draw_cursor_y++;
        if(g_draw_cursor_y >= g_play_area_h) {
          g_draw_cursor_y = g_play_area_h -1;
          g_pic_render_y++;
          g_components.render_main_area_squares = 1;
        }
      }
      /* Check to make sure the visible area is fully in the picture */
      if (g_pic_render_y >= g_picture->h - g_play_area_h) {
        g_pic_render_y = g_picture->h - g_play_area_h;
      }
      /* Calculate where we'll be drawing within the whole picture */        
      g_draw_position_x = g_pic_render_x + g_draw_cursor_x;
      g_draw_position_y = g_pic_render_y + g_draw_cursor_y;

      g_components.render_draw_cursor = 1;
      g_components.render_scrollbars = 1;  
      g_components.render_overview_display = 1;        
      g_keypress_lockout[KEY_DOWN] = 1;
    }
  }
  if (!key[KEY_DOWN] && g_keypress_lockout[KEY_DOWN]) {
    g_keypress_lockout[KEY_DOWN] = 0;
  }

  /*-------------------------------------------------------------------------
   * Space - Mark the highlighted square with the current color
   *------------------------------------------------------------------------*/
  if (key[KEY_SPACE]) {
    if (!g_keypress_lockout[KEY_SPACE]) {
      square_offset = (g_draw_position_y * g_picture->w) +
                       g_draw_position_x;
      fill_val = g_picture->pic_squares[square_offset].fill_value;
      pal_val = g_picture->pic_squares[square_offset].pal_entry;

      /* If unfilled, fill with the active color.  If filled, unfill it,
        but only if it's the incorrect color. */
      if(fill_val != 0) {
        if (fill_val != pal_val) {
          g_picture->pic_squares[square_offset].fill_value = 0;           
          g_picture->mistakes[square_offset] = 0;
          g_mistake_count--;
          g_picture->pic_squares[square_offset].correct = 0;          
        }
     } else {
        g_picture->pic_squares[square_offset].fill_value = g_cur_color;
        /* Update mistake/progress counters */           
        if (g_cur_color != pal_val) {
          g_picture->mistakes[square_offset] = g_cur_color;
          g_mistake_count++;
          g_picture->pic_squares[square_offset].correct = 0;          
        }
        else {
          g_picture->draw_order[g_correct_count].x = g_draw_position_x;
          g_picture->draw_order[g_correct_count].y = g_draw_position_y;
          g_correct_count++;
          g_picture->pic_squares[square_offset].correct = 1;
          /* Check to see if we're done with the picture */
          done = check_completion();
          if (done)
           change_state(STATE_FINISHED, STATE_GAME);
        }                     
     }
     clear_render_components(&g_components);
     update_overview_area_at((g_draw_position_x - 
                            (g_draw_position_x % OVERVIEW_BLOCK_SIZE)) /
                            OVERVIEW_BLOCK_SIZE,
                            (g_draw_position_y - 
                             (g_draw_position_y % OVERVIEW_BLOCK_SIZE)) /
                             OVERVIEW_BLOCK_SIZE);
     g_components.render_draw_cursor = 1;
     g_components.render_status_text = 1;
     g_components.render_overview_display = 1;
     g_keypress_lockout[KEY_SPACE] = 1;
    }
  }
  if(!key[KEY_SPACE] && g_keypress_lockout[KEY_SPACE]) {
   g_keypress_lockout[KEY_SPACE] = 0;
  }
}

/*=============================================================================
 * process_main_area_mouse_input
 *============================================================================*/
void process_main_area_mouse_input(void) {

  int square_offset, fill_val, pal_val, done;
  Position p;

  g_old_mouse_x = g_mouse_x;
  g_old_mouse_y = g_mouse_y;

  g_mouse_x = mouse_x;
  g_mouse_y = mouse_y;

  if(g_mouse_x != g_old_mouse_x || g_mouse_y != g_old_mouse_y || (mouse_b & 1)) {
        g_keyboard_has_priority = 0;
  } else {
        g_keyboard_has_priority = 1;
  }

  /* If in the game area */
  if (is_in_game_area(g_mouse_x, g_mouse_y) && !g_keyboard_has_priority) {
    /* Get all the relevant positional info */
    p = get_square_at(g_mouse_x, g_mouse_y);
    g_old_draw_cursor_x = g_draw_cursor_x;
    g_old_draw_cursor_y = g_draw_cursor_y;    
    g_draw_cursor_x = p.x;
    g_draw_cursor_y = p.y;
    g_draw_position_x = g_pic_render_x + g_draw_cursor_x;
    g_draw_position_y = g_pic_render_y + g_draw_cursor_y;
    square_offset = (g_draw_position_y * g_picture->w) +
                     g_draw_position_x;
    fill_val = g_picture->pic_squares[square_offset].fill_value;
    pal_val = g_picture->pic_squares[square_offset].pal_entry;

    /* If we're in neutral mode and clicking over empty space or correctly 
       filled space, enter draw mode */
    if (mouse_b & 1) {
      if (g_game_area_mouse_mode == MOUSE_MODE_NEUTRAL) {
        if (fill_val == 0  || (g_picture->pic_squares[square_offset].correct)) {
          g_game_area_mouse_mode = MOUSE_MODE_DRAW;
        }
        else if (!g_picture->pic_squares[square_offset].correct) {
          g_game_area_mouse_mode = MOUSE_MODE_ERASE;
        }
      }
      /* If in draw mode, draw in the space if it isn't drawn yet */      
      if (g_game_area_mouse_mode == MOUSE_MODE_DRAW) {         
        /* Update mistake/progress counters */                  
        if (g_cur_color != pal_val && g_picture->mistakes[square_offset] == 0) {          
            if(g_picture->pic_squares[square_offset].correct == 0) {
              g_picture->pic_squares[square_offset].fill_value = g_cur_color;              
              g_picture->mistakes[square_offset] = g_cur_color;
              g_mistake_count++;
              g_picture->pic_squares[square_offset].correct = 0;
            } 
            else {
              g_picture->pic_squares[square_offset].correct = 1;              
            }
        }             
        if (fill_val == 0 && g_cur_color == pal_val) {
          g_picture->pic_squares[square_offset].fill_value = g_cur_color;          
          g_picture->draw_order[g_correct_count].x = g_draw_position_x;
          g_picture->draw_order[g_correct_count].y = g_draw_position_y;       
          g_picture->mistakes[square_offset] = 0;
          g_correct_count++;
          g_picture->pic_squares[square_offset].correct = 1;
          /* Check to see if we're done with the picture */
          done = check_completion();
          if (done)
           change_state(STATE_FINISHED, STATE_GAME);
        }                     
      }      
      /* If in erase mode, erase the space if it's drawn incorrectly */      
      if (g_game_area_mouse_mode == MOUSE_MODE_ERASE) {
        if (!g_picture->pic_squares[square_offset].correct && g_picture->mistakes[square_offset] != 0) {
          g_picture->pic_squares[square_offset].fill_value = 0;           
          g_picture->mistakes[square_offset] = 0;
          g_picture->pic_squares[square_offset].correct = 0;
          g_mistake_count--;
        }
      }      
      clear_render_components(&g_components);
      update_overview_area_at((g_draw_position_x - 
                             (g_draw_position_x % OVERVIEW_BLOCK_SIZE)) /
                             OVERVIEW_BLOCK_SIZE,
                             (g_draw_position_y - 
                              (g_draw_position_y % OVERVIEW_BLOCK_SIZE)) /
                              OVERVIEW_BLOCK_SIZE);                       
    }      
    g_components.render_draw_cursor = 1;
    g_components.render_status_text = 1;
    g_components.render_overview_display = 1;           
  } /* is_in_game_area */  

  if (is_in_overview_area(g_mouse_x, g_mouse_y)) {
    int click_image_x = (g_mouse_x - OVERVIEW_X) * OVERVIEW_BLOCK_SIZE;
    int click_image_y = (g_mouse_y - OVERVIEW_Y) * OVERVIEW_BLOCK_SIZE;

    if (mouse_b & 1) {
      // Does the click represent an in-image region?
      if (click_image_x < g_picture->w && click_image_y <= g_picture->h) {
        g_pic_render_x = click_image_x;
        g_pic_render_y = click_image_y;
        /* CLamp the position into the play area */
        if (g_pic_render_y >= g_picture->h - g_play_area_h) {
          g_pic_render_y = g_picture->h - g_play_area_h;
        }
        if (g_pic_render_x >= g_picture->w - g_play_area_w) {
          g_pic_render_x = g_picture->w - g_play_area_w;
        }        
        /* Figure out where to start drawing from */
        g_draw_position_x = g_pic_render_x + g_draw_cursor_x;
        g_draw_position_y = g_pic_render_y + g_draw_cursor_y;    
        clear_render_components(&g_components);
        update_overview_area_at((g_draw_position_x - 
                            (g_draw_position_x % OVERVIEW_BLOCK_SIZE)) /
                            OVERVIEW_BLOCK_SIZE,
                            (g_draw_position_y - 
                             (g_draw_position_y % OVERVIEW_BLOCK_SIZE)) /
                             OVERVIEW_BLOCK_SIZE);               
        g_components.render_scrollbars = 1;                
      }    
    }
    g_components.render_draw_cursor = 1;
    g_components.render_status_text = 1;
    g_components.render_overview_display = 1;
    g_components.render_main_area_squares = 1;       
  }

}

/*=============================================================================
 * input_state_help
 *============================================================================*/
void input_state_help(void) {

  /* ESC - exit help */
   if (key[KEY_ESC]) {
    if (!g_keypress_lockout[KEY_ESC]) {
        clear_keybuf();        
        change_state(STATE_GAME, STATE_HELP);
        g_keypress_lockout[KEY_ESC] = 1;          
      }
    }
    if (!key[KEY_ESC] && g_keypress_lockout[KEY_ESC]) {
      g_keypress_lockout[KEY_ESC] = 0;
    }   

   if (key[KEY_X]) {
    if (!g_keypress_lockout[KEY_E]) {
        clear_keybuf();        
        change_state(STATE_GAME, STATE_HELP);
        g_keypress_lockout[KEY_E] = 1;          
      }
    }
    if (!key[KEY_E] && g_keypress_lockout[KEY_E]) {
      g_keypress_lockout[KEY_E] = 0;
    }

   /* P or left arrow - previous help page */
   if (key[KEY_P]) {
    if (!g_keypress_lockout[KEY_P]) {
        g_help_page--;
        if(g_help_page < 0)
          g_help_page = 0;          
        g_keypress_lockout[KEY_P] = 1;
      }
    }
    if (!key[KEY_P] && g_keypress_lockout[KEY_P]) {
      g_keypress_lockout[KEY_P] = 0;
    }      
   if (key[KEY_LEFT]) {
    if (!g_keypress_lockout[KEY_LEFT]) {
        g_help_page--;
        if(g_help_page < 0)
          g_help_page = 0;          
        g_keypress_lockout[KEY_LEFT] = 1;          
      }
    }
    if (!key[KEY_LEFT] && g_keypress_lockout[KEY_LEFT]) {
      g_keypress_lockout[KEY_LEFT] = 0;
    }   

  /* N or right arrow - next help page */
  if (key[KEY_N]) {
    if (!g_keypress_lockout[KEY_N]) {
        g_help_page++;
        if(g_help_page >= MAX_HELP_PAGES)
          g_help_page = MAX_HELP_PAGES - 1;          
        g_keypress_lockout[KEY_N] = 1;          
      }
  }
  if (!key[KEY_N] && g_keypress_lockout[KEY_N]) {
    g_keypress_lockout[KEY_N] = 0;
  }     
  if (key[KEY_RIGHT]) {
    if (!g_keypress_lockout[KEY_RIGHT]) {
      g_help_page++;
      if(g_help_page >= MAX_HELP_PAGES)
        g_help_page = MAX_HELP_PAGES - 1;          
      g_keypress_lockout[KEY_RIGHT] = 1;          
    }
  }
  if (!key[KEY_RIGHT] && g_keypress_lockout[KEY_RIGHT]) {
    g_keypress_lockout[KEY_RIGHT] = 0;
  }   

  /* If Previous Page is clicked */
  if(mouse_clicked_here(15, 185, 97, 195, 1)) {
    g_help_page--;
    if(g_help_page < 0)
      g_help_page = 0;     
  }

  /* If Previous Page is clicked */
  if(mouse_clicked_here(117, 185, 199, 195, 1)) {
        change_state(STATE_GAME, STATE_HELP);
  }

  /* If Next Page is clicked */
  if(mouse_clicked_here(220, 185, 302, 195, 1)) {
    g_help_page++;
    if(g_help_page >= MAX_HELP_PAGES)
      g_help_page = MAX_HELP_PAGES - 1;       
  }
}

/*=============================================================================
 * input_state_load_dialog
 *============================================================================*/
void input_state_load_dialog(void) {

    if (key[KEY_ENTER]) {
      if (!g_keypress_lockout[KEY_ENTER]) {
        /* Only load an image if the image side is highlighted */
        if (g_load_section_active == LOAD_IMAGE_ACTIVE) {
          strncpy(g_picture_file_basename, 
                  g_pic_items[g_load_picture_index].name, 8);
          g_load_new_file = 1;
          change_state(STATE_GAME, STATE_LOAD_DIALOG);
        }
        g_keypress_lockout[KEY_ENTER] = 1;          
      }
    }
    if (!key[KEY_ENTER] && g_keypress_lockout[KEY_ENTER]) {
      g_keypress_lockout[KEY_ENTER] = 0;
    }    

    /* TAB - select between collection and image tabs */
    if (key[KEY_TAB]) {
      if (!g_keypress_lockout[KEY_TAB]) {
        if (g_load_section_active == LOAD_COLLECTION_ACTIVE &&
            g_num_picture_files > 0) {
          g_load_section_active = LOAD_IMAGE_ACTIVE;
        } else {
          g_load_section_active = LOAD_COLLECTION_ACTIVE;
        }
      }
      g_keypress_lockout[KEY_TAB] = 1;
    }
    if (!key[KEY_TAB] && g_keypress_lockout[KEY_TAB]) {
      g_keypress_lockout[KEY_TAB] = 0;
    }

    if (key[KEY_ESC]) {
      if (!g_keypress_lockout[KEY_ESC]) {
        /* Change to the appropriate state.  Could be the title screen
           or the game screen depending on where the dialog was invoked.
           The appropriate state is actually held in g_prev_state. */
        g_load_new_file = 0;
        clear_keybuf();        
        change_state(g_prev_state, STATE_LOAD_DIALOG);
        g_keypress_lockout[KEY_ESC] = 1;          
      }
    }
    if (!key[KEY_ESC] && g_keypress_lockout[KEY_ESC]) {
      g_keypress_lockout[KEY_ESC] = 0;
    }   

    /* Need the following */
    /* index - the actual value of the picture to load */
    /* offset - the index of the top position on the dialog */
    /* dialog_index - the position of the highlighted entry in the dialog */

    if (key[KEY_DOWN]) {
      /* If the key was previously up */
      if (!g_keypress_lockout[KEY_DOWN]) {
        /* Adjust the image cursor if the image tab is active */
        if (g_load_section_active == LOAD_IMAGE_ACTIVE) {
          g_load_picture_index++;
          if (g_load_picture_index >= g_num_picture_files) {
            g_load_picture_index = g_num_picture_files -1;
          }
          else {
            g_load_cursor_offset++;
            if(g_load_cursor_offset >= LOAD_NUM_VISIBLE_FILES) {
              g_load_picture_offset++;            
              g_load_cursor_offset = LOAD_NUM_VISIBLE_FILES -1;
            }
          }
        }
        /* Adjust the collection cursor if the collection tab is active */
        if (g_load_section_active == LOAD_COLLECTION_ACTIVE) {
          g_load_collection_index++;
          if (g_load_collection_index >= g_num_collections) {
            g_load_collection_index = g_num_collections -1;
          }
          else {
            g_load_collection_cursor_offset++;
            if(g_load_collection_cursor_offset >= LOAD_NUM_VISIBLE_FILES) {
              g_load_collection_offset++;            
              g_load_collection_cursor_offset = LOAD_NUM_VISIBLE_FILES -1;
            }
          }
        }        
      }

      /* Update the images in the collection */
      if(g_load_section_active == LOAD_COLLECTION_ACTIVE) {
        get_picture_files(g_collection_items[g_load_collection_index].name);
        g_load_picture_offset = 0;
        g_load_cursor_offset = 0;
        g_load_picture_index = 0;
      }

      g_keypress_lockout[KEY_DOWN] = 1;      
    }
    if (!key[KEY_DOWN] && g_keypress_lockout[KEY_DOWN]) {
      g_keypress_lockout[KEY_DOWN] = 0;
    }

    if (key[KEY_UP]) {
      if (g_load_section_active == LOAD_IMAGE_ACTIVE) {
        /* If the key was previously up */
        if (!g_keypress_lockout[KEY_UP]) {
          g_load_picture_index--;
          if (g_load_picture_index < 0) {
            g_load_picture_index = 0;
          }
          else {
            g_load_cursor_offset--;
            if(g_load_cursor_offset < 0) {
              g_load_picture_offset--;
              g_load_cursor_offset = 0;
            } 
          } 
        }
      } 
      if (g_load_section_active == LOAD_COLLECTION_ACTIVE) {
        /* If the key was previously up */
        if (!g_keypress_lockout[KEY_UP]) {
          g_load_collection_index--;
          if (g_load_collection_index < 0) {
            g_load_collection_index = 0;
          }
          else {
            g_load_collection_cursor_offset--;
            if(g_load_collection_cursor_offset < 0) {
              g_load_collection_offset--;
              g_load_collection_cursor_offset = 0;
            } 
          } 
        }         
      }

      /* Update the images in the collection */
      if(g_load_section_active == LOAD_COLLECTION_ACTIVE) {
        get_picture_files(g_collection_items[g_load_collection_index].name);
        g_load_picture_offset = 0;
        g_load_cursor_offset = 0;
        g_load_picture_index = 0;
      }

      g_keypress_lockout[KEY_UP] = 1;      
    }
    if (!key[KEY_UP] && g_keypress_lockout[KEY_UP]) {
      g_keypress_lockout[KEY_UP] = 0;
    }
}

/*=============================================================================
 * input_state_logo
 *============================================================================*/
void input_state_logo(void) {
    if (mouse_clicked_here(0, 0, 319, 199, 1)) {
      change_state(STATE_TITLE, STATE_LOGO);
    }

/* Catch either the space bar or ENTER key, and if pressed, move to the title
   screen */
  if (key[KEY_ENTER]) {
    if(!g_keypress_lockout[KEY_ENTER]) {
      change_state(STATE_TITLE, STATE_LOGO);          
      g_keypress_lockout[KEY_ENTER] = 1;
    }
  }
  if (!key[KEY_ENTER] && g_keypress_lockout[KEY_ENTER]) {
    g_keypress_lockout[KEY_ENTER] = 0;
  }       

  if (key[KEY_SPACE]) {
    if(!g_keypress_lockout[KEY_SPACE]) {
      change_state(STATE_TITLE, STATE_LOGO);          
      g_keypress_lockout[KEY_SPACE] = 1;
    }
  }
  if (!key[KEY_SPACE] && g_keypress_lockout[KEY_SPACE]) {
    g_keypress_lockout[KEY_SPACE] = 0;
  }        
}

/*=============================================================================
 * input_state_title
 *============================================================================*/
void input_state_title(void) {

  if (mouse_clicked_here(0, 0, 319, 199, 1)) {
      change_state(STATE_LOAD_DIALOG, STATE_TITLE);
  }

  if (key[KEY_ENTER]) {
    if(!g_keypress_lockout[KEY_ENTER]) {
      change_state(STATE_LOAD_DIALOG, STATE_TITLE);          
      g_keypress_lockout[KEY_ENTER] = 1;
    }
  }
  if (!key[KEY_ENTER] && g_keypress_lockout[KEY_ENTER]) {
    g_keypress_lockout[KEY_ENTER] = 0;
  }      

  if (key[KEY_ESC]) {
    if (!g_keypress_lockout[KEY_ESC]) {
        g_game_done = 1;      
    }
  }
  if (!key[KEY_ESC] && g_keypress_lockout[KEY_ESC]) {
    g_keypress_lockout[KEY_ESC] = 0;
  }      
}

/*=============================================================================
 * input_state_map
 *============================================================================*/
void input_state_map(void) {

    /* did the mouse click the exit button when it's showing? */
    if(mouse_clicked_here(118, 186, 200, 199, 1)) {
        if (g_show_map_text == 1) {
          change_state(STATE_GAME, STATE_MAP);
        }
        else {
          g_show_map_text = 1;
          clear_render_components(&g_components);
          g_components.render_map = 1;          
        }
    }
    /* Did the mouse click anywhere else? */
    else if (mouse_clicked_here(0, 0, 319, 199, 1)) {
        if (g_show_map_text == 0) {
          g_show_map_text = 1;
        } else {
          g_show_map_text = 0;
        }
        clear_render_components(&g_components);
        g_components.render_map = 1;
    }

    if (key[KEY_M]) {
      if (!g_keypress_lockout[KEY_M]) {
        change_state(STATE_GAME, STATE_MAP);
        g_keypress_lockout[KEY_M] = 1;          
      }
    }
    if (!key[KEY_M] && g_keypress_lockout[KEY_M]) {
      g_keypress_lockout[KEY_M] = 0;
    }    

    if (key[KEY_ESC]) {
      if (!g_keypress_lockout[KEY_ESC]) {
        change_state(STATE_GAME, STATE_MAP);
        g_keypress_lockout[KEY_ESC] = 1;          
      }
    }
    if (!key[KEY_ESC] && g_keypress_lockout[KEY_ESC]) {
      g_keypress_lockout[KEY_ESC] = 0;
    }    

    if (key[KEY_C]) {
      if (!g_keypress_lockout[KEY_C]) {
        if (g_show_map_text == 0) {
          g_show_map_text = 1;
        } else {
          g_show_map_text = 0;
        }
        clear_render_components(&g_components);
        g_components.render_map = 1;
        g_keypress_lockout[KEY_C] = 1;          
      }
    }
    if (!key[KEY_C] && g_keypress_lockout[KEY_C]) {
      g_keypress_lockout[KEY_C] = 0;
    }        
}

/*=============================================================================
 * input_state_game
 *============================================================================*/
void input_state_game(void) {

    /* Process each in-game action (both keyboard and mouse) */
    process_palette_press();
    process_palette_color_press();
    process_mark_press();
    process_help_press();
    process_exit_press();
    process_map_press();
    process_style_press();
    process_save_press();
    process_load_press();
    // Only process keyboard movement if the keyboard has priority.
    // The mouse input function will confirm its own priority since it needs
    // to check to see if it's moving or the button is clicked first.
    if (g_keyboard_has_priority)
      process_main_area_keyboard_input();
    process_main_area_mouse_input();
}

/*=============================================================================
 * input_state_replay
 *============================================================================*/
void input_state_replay(void) {

  if (mouse_clicked_here(0, 0, 319, 199, 1)) {
    change_state(STATE_TITLE, STATE_REPLAY);
  }

  if (key[KEY_ENTER]) {
    if(!g_keypress_lockout[KEY_ENTER]) {
      change_state(STATE_TITLE, STATE_REPLAY);          
      g_keypress_lockout[KEY_ENTER] = 1;
    }
  }
  if (!key[KEY_ENTER] && g_keypress_lockout[KEY_ENTER]) {
    g_keypress_lockout[KEY_ENTER] = 0;
  }     
}