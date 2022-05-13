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
#ifndef __RENDER_H__
#define __RENDER_H__

#define NEW_COLOR_COUNT   1
#define NEW_NUMBER_COUNT  12

/* A list of areas of the screen to update when calling render_screen() */
typedef struct {
  /* The visible part of the work area*/
  int render_main_area_squares;
  /* The palette columns on the right */
  int render_palette_area;
  /* The numbers/colors in the palette */
  int render_palette;
  /* All static UI elements */
  int render_ui_components;
  /* Menu buttons */
  int render_buttons;
  /* The overview area in the top right */
  int render_overview_display;
  /* The time/progress text at the bottom */
  int render_status_text;
  /* Update the draw cursor */
  int render_draw_cursor;
  /* Update the palette cursor */
  int render_palette_cursor;
  /* Update the scrollbars */
  int render_scrollbars;
  /* The map on the map screen */
  int render_map;

  /* Render everything - convenience function */
  int render_all;
  /* Render debug text.  Must be explicitly set or cleared */
  int render_debug;
} RenderComponents;

typedef struct {
  char update_background;
  char new_color_counter;
  char color_start_counter;
  char update_title_color;
  char color_start;
} TitleAnimation;

typedef enum {
  STYLE_SOLID,
  STYLE_DIAMOND,
  STYLE_CROSS,
  NUM_STYLES
} Style;

/*=============================================================================
 * clear_render_components
 *
 * Resets all of the render flags for the display.  
 *============================================================================*/
void clear_render_components(RenderComponents *c);

/*=============================================================================
 * render_logo
 *
 * Draws the logo.
 *============================================================================*/
void render_logo(BITMAP *dest, RenderComponents c);

/*=============================================================================
 * update_scrollbar_positions
 *
 * Determines where and how the scrollbars should be drawn
 *============================================================================*/
void update_scrollbar_positions(void);

/*=============================================================================
 * render_scrollbars
 *
 * Draws the scrollbars
 *============================================================================*/
void render_scrollbars(BITMAP *dest);

/*=============================================================================
 * render_main_area_square_at
 *
 * Draws content of a specific square of the play area (either a number if
 * the square isn't filled in, otherwise a block of color representing the
 * color that has been filled in).
 * 
 * - tl_x and tl_y refer to the position of the picture that makes up the upper
 *   left part of the play area
 * - off_x and off_y refer to the position within the play area to draw
 *============================================================================*/
void render_main_area_square_at(BITMAP *dest, int tl_x, int tl_y,
                               int off_x, int off_y);

/*=============================================================================
 * render_palette_item_at
 *
 * Draws content of a specific index of the palette.  If change_page is set,
 * the page will be adjusted as well.  Otherwise, it will just draw the
 * specified index number and swatch at the place where it would normally go.
 *============================================================================*/

void render_palette_item_at(BITMAP *dest, int palette_index, int change_page);

/*=============================================================================
 * render_status_text
 *
 * Draws the status text in the lower portion of the screen 
 *============================================================================*/
void render_status_text(BITMAP *dest);

/*=============================================================================
 * render_primary_ui
 *
 * Draws all components that aren't dynamic somehow
 *============================================================================*/
void render_primary_ui(BITMAP *dest);

/*=============================================================================
 * render_menu_buttons
 *
 * Draws all buttons in the lower right corner of the screen
 *============================================================================*/
void render_menu_buttons(BITMAP *dest);

/*=============================================================================
 * render_map_screen
 *
 * Draws screen components when the game is in the STATE_MAP state.
 *============================================================================*/
void render_map_screen(BITMAP *dest, RenderComponents c);

/*=============================================================================
 * render_title_screen
 *
 * Draws screen components when the game is in the STATE_TITLE state.
 *============================================================================*/
void render_title_screen(BITMAP *dest, RenderComponents c);

/*=============================================================================
 * render_game_screen
 *
 * Draws screen components when the game is in the STATE_GAME state.
 *============================================================================*/
void render_game_screen(BITMAP *dest, RenderComponents c);

/*=============================================================================
 * render_screen
 *
 * Draws screen components.  The components that are drawn is dictated by
 * the current state and the global RenderComponents object.
 *============================================================================*/
void render_screen(BITMAP *dest, RenderComponents c);

/*=============================================================================
 * get_prop_text_width
 *
 * Gets the width of a proportional string.
 *============================================================================*/
int get_prop_text_width(char *text);

/*=============================================================================
 * render_centered_prop_text
 *
 * Writes a string centered at 'center' using the proportional font.
 *============================================================================*/
void render_centered_prop_text(BITMAP *dest, char *text, int center, int y_pos);

/*=============================================================================
 * render_prop_text
 *
 * Writes a string using the proportional font
 *============================================================================*/
void render_prop_text(BITMAP *destination, char *text, int x_pos, int y_pos);

/*=============================================================================
 * render_load_dialog
 *
 * Displays the 'load picture' dialog
 *============================================================================*/
void render_load_dialog(BITMAP *dest, RenderComponents c);

/*=============================================================================
 * render_save_message
 *
 * Displays the 'saving' message
 *============================================================================*/
void render_save_message(BITMAP *dest, RenderComponents c);

/*=============================================================================
 * render_load_message
 *
 * Displays the 'loading' message
 *============================================================================*/
void render_load_message(BITMAP *dest, RenderComponents c);

/*=============================================================================
 * render_help_text
 *
 * Displays the current help page
 *============================================================================*/
void render_help_text(BITMAP *dest, RenderComponents c);

/*=============================================================================
 * render_replay_state
 *
 * Displays the replay
 *============================================================================*/
void render_replay_state(BITMAP *dest, RenderComponents c);

/*=============================================================================
 * update_overview_area
 *
 * Update the underlying overview area bitmap. Used to seed the entire thing
 * after loading an in-progress picture
 *============================================================================*/
void update_overview_area(void);

/*=============================================================================
 * update_overview_area_at
 *
 * Update a single block (4x4 pixels) of the overview area.  Used by 
 * update_overview_area and called when a block is placed somewhere
 *============================================================================*/
void update_overview_area_at(int i, int j);

/*=============================================================================
 * load_title
 *
 * Load title graphics.
 *============================================================================*/
int load_title(void);

/*=============================================================================
 * load_logo
 *
 * Load logo graphics.
 *============================================================================*/
int load_logo(void);

/*=============================================================================
 * free_graphics
 *
 * Destroy bitmaps created with create_bitmap()
 *============================================================================*/
void free_graphics(void);

/*=============================================================================
 * load_graphics
 *
 * Load in-game graphics into bitmap objects.  All graphics are stored in the
 * PCX format.
 *============================================================================*/
int load_graphics(void);

#endif
