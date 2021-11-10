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
#ifndef __DAMPBN_H__
#define __DAMPBN_H__
#include <allegro.h>

/* The size of the area the player colors in, in squares */
#define PLAY_AREA_W       20
#define PLAY_AREA_H       16

/* The maximum number of colors in the palette */
#define MAX_COLORS        64

/* The number of palette colors displayed at once */
#define PALETTE_COLORS_PER_PAGE     32

#define FIRST_COLOR_ON_FIRST_PAGE    1
#define LAST_COLOR_ON_FIRST_PAGE    32
#define FIRST_COLOR_ON_SECOND_PAGE  33
#define LAST_COLOR_ON_SECOND_PAGE   64

/* The internal frame rate of various timers */
#define FRAME_RATE        30

/* Compression types for dampbm .PIC files */
#define COMPRESSION_NONE   0
#define COMPRESSION_RLE    1

/* Bitmaps */
BITMAP *g_numbers;
BITMAP *g_bg_lower;
BITMAP *g_bg_right;
BITMAP *g_mainarea;
BITMAP *g_pal_col;
BITMAP *g_draw_cursor;
BITMAP *g_small_pal;
BITMAP *g_large_pal;
BITMAP *g_pal_cursor;
BITMAP *g_wrong;

/* The position of the cursor within the draw area */
int g_draw_cursor_x;
int g_draw_cursor_y;

int g_old_draw_cursor_x;
int g_old_draw_cursor_y;

/* The position of the picture that's at the top left corner of 
 * the draw area */
int g_pic_render_x;
int g_pic_render_y;

/* The position that will be drawn within the picture at the cursor */
int g_draw_position_x;
int g_draw_position_y;

/* Has the player exited the game? */
int g_game_done;

/* What page of the palette is visible? */
int g_palette_page;

/* What color will be used to draw */
int g_cur_color;

/* The last color selected.  Used to limit what's being drawn/erased */
int g_prev_color;

/* An array of key codes.  Used to prevent keys from retriggering
 * until released */
unsigned char g_keypress_lockout[128];

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
  /* The overview area in the top right */
  int render_overview_display;
  /* The time/progress text at the bottom */
  int render_status_text;
  /* Update the draw cursor */
  int render_draw_cursor;
  /* Update the palette cursor */
  int render_palette_cursor;
  /* Render everything - convenience function */
  int render_all;
  /* Render debug text.  Must be explicitly set or cleared */
  int render_debug;
} RenderComponents;

typedef struct {
  /* The color this square should be */
  char pal_entry;
  /* 0 = not filled in, x = value actually filled in */
  char fill_value;
  /* The order in which this square was *correctly* filled in */
  int  order;
} ColorSquare;

typedef struct {
  /* The x and y position of the Nth square filled in correctly */
  int x;
  int y;
} OrderItem;

typedef struct {
  char image_name[32];
  short w;
  short h;
  unsigned char category;
  unsigned char num_colors;
  ColorSquare *pic_squares;
  OrderItem *draw_order;
} Picture;

RenderComponents g_components;
Picture *g_picture;

/* == Functions from input.c == */

/*=============================================================================
 * process_input
 *
 * Take keypresses and perform the appropriate actions.  'Appropriate' actions
 * vary based on the game state.
 *============================================================================*/
int process_input(int state);

/* == Functions from dampbn.c == */

/*=============================================================================
 * load_picture_file
 *
 * Loads an image created using the dampbn convert tool into an appropriate
 * instance of the Picture data structure.  
 *============================================================================*/
Picture *load_picture_file(char *filename);

/*=============================================================================
 * free_picture_file
 *
 * Deallocates all objects associated with a picture file.
 *============================================================================*/
void free_picture_file(Picture *p);

/*=============================================================================
 * clear_render_components
 *
 * Resets all of the render flags for the display.  
 *============================================================================*/
void clear_render_components(RenderComponents *c);

/*=============================================================================
 * load_palette_swatches
 *
 * Populates the 2 palette swatch bitmaps (one for the main draw area, one
 * for the color select area) with the colors used by the current Picture.  
 *============================================================================*/
void load_palette_swatches(void);

/*=============================================================================
 * int_handler
 *
 * An interrupt handler called 30 times per second.  Not currently used for
 * anything but will be used for progress timing.
 *============================================================================*/
void int_handler(void);

/*=============================================================================
 * init_defaults
 *
 * Resets various variables to their default values
 *============================================================================*/
void init_defaults(void);

/*=============================================================================
 * render_main_area_squares
 *
 * Draws content for each of the squares of the play area (either a number,
 * if the square isn't filled in, otherwise a block of color respresenting the
 * color that has been filled in).
 * 
 * - x_off and y_off refer to the position of the picture that makes up the
 *   upper left part of the play area
 *============================================================================*/
void render_main_area_squares(BITMAP *dest, int x_off, int y_off);

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
 * render_screen
 *
 * Draws screen components.  The components that are drawn is dictated by
 * the global RenderComponents object.
 *============================================================================*/
void render_screen(BITMAP *dest, RenderComponents c);

/*=============================================================================
 * load_graphics
 *
 * Load in-game graphics into bitmap objects.  All graphics are stored in the
 * PCX format.
 *============================================================================*/
int load_graphics(void);

/*=============================================================================
 * destroy_graphics
 *
 * Free memory used by loaded bitmaps.
 *============================================================================*/
void destroy_graphics(void);

#endif
