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
#ifndef __GLOBALS_H__
#define __GLOBALS_H__
#include "../include/dampbn.h"
#include "../include/render.h"
#include "../include/util.h"
#include "../include/palette.h"
#include "../include/uiconsts.h"
#include "../include/input.h"

/* Compression types for dampbm .PIC files */
#define COMPRESSION_NONE   0
#define COMPRESSION_RLE    1

#define MAX_PLAY_AREA_WIDTH  20
#define MAX_PLAY_AREA_HEIGHT 16

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

/* Width and height of all characters from ASCII values 32 to 127 in the
   standard proportional font*/
extern int g_prop_font_width[];
extern int g_prop_font_height;

/* Offsets into the font bitmap for each character (based on the character 
   widths defined in prop_font_width) */
extern int prop_font_offset[];

/* A flag used to notify the game loop that the screen should be updated */
extern int g_update_screen;

/* A counter used by the interrupt to update the status screen */
extern volatile unsigned long g_int_counter;

/* The elapsed play time for the loaded picture */
extern volatile unsigned int g_elapsed_time;

/* Should the elapsed time be advancing? */
extern int g_game_timer_running;

/* The list of picture categories as stored in a Picture's headers */
extern char *g_categories[];

/* An array of key codes.  Used to prevent keys from retriggering
 * until released */
extern unsigned char g_keypress_lockout[];

/* Image bitmaps used by the game */
extern BITMAP *g_logo;

extern BITMAP *g_numbers;
extern BITMAP *g_highlight_numbers;
extern BITMAP *g_bg_lower;
extern BITMAP *g_bg_right;
extern BITMAP *g_mainarea;
extern BITMAP *g_draw_cursor;
extern BITMAP *g_small_pal;
extern BITMAP *g_large_pal;
extern BITMAP *g_large_diamonds;
extern BITMAP *g_large_crosses;
extern BITMAP *g_pal_cursor;
extern BITMAP *g_wrong;
extern BITMAP *g_page_buttons;
extern BITMAP *g_main_buttons;
extern BITMAP *g_prop_font;

/* The parts of the screen to render */
extern RenderComponents g_components;

/* The currently active Picture */
extern Picture *g_picture;

/* The current game state */
extern State g_state;

/* The previous game state */
extern State g_prev_state;

/* Has the player exited the game? */
extern int g_game_done;

/* The position of the cursor within the draw area */
extern int g_draw_cursor_x;
extern int g_draw_cursor_y;

extern int g_old_draw_cursor_x;
extern int g_old_draw_cursor_y;

/* The position of the picture that's at the top left corner of 
 * the draw area */
extern int g_pic_render_x;
extern int g_pic_render_y;

/* The position that will be drawn within the picture at the cursor */
extern int g_draw_position_x;
extern int g_draw_position_y;

/* What page of the palette is visible? */
extern int g_palette_page;

/* What color will be used to draw */
extern int g_cur_color;

/* The last color selected.  Used to limit what's being drawn/erased */
extern int g_prev_color;

/* Number of squares marked incorrectly */
extern int g_mistake_count;

/* Number of squares marked correctly */
extern int g_correct_count;

/* Total number of squares in the picture (for convenience) */
extern int g_total_picture_squares;

/* The parts of the screen to render */
extern RenderComponents g_components;

/* The active Picture */
extern Picture *g_picture;

/* The size of the play area.  If the picture is smaller than the visible area,
   these values will be set to the size of the picture, otherwise they will
   be set to the size of the visible play area */
extern int g_play_area_w;
extern int g_play_area_h;

/* If set, all squares of the active color will be highlighted for easy
   identification. */
extern int g_mark_current;

/* Size and position of the play area scrollbar in the x direction */
extern int g_across_scrollbar_x;
extern int g_across_scrollbar_width;

/* Size and position of the play area scrollbar in the y direction */
extern int g_down_scrollbar_y;
extern int g_down_scrollbar_height;

/* Should the text on the map display be shown? */
extern int g_show_map_text;

/* The style of square to draw with */
extern int g_draw_style;

/* A flag to change stuff once a second rolls over - avoids doing work in
   the interrupt handler */
extern int g_seconds_update;

/* Timer to automatically move from logo to title screen if no key is pressed */
extern int g_title_countdown;
#endif
