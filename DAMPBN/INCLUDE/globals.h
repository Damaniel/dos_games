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
#include "../include/res.h"

#define LOAD_COLLECTION_ACTIVE   0
#define LOAD_IMAGE_ACTIVE        1

/* Compression types for dampbm .PIC files */
#define COMPRESSION_NONE   0
#define COMPRESSION_RLE    1

#define MAX_PLAY_AREA_WIDTH  20
#define MAX_PLAY_AREA_HEIGHT 16

/* The maximum number of colors in the palette */
#define MAX_COLORS        64

/* How many pages of help are there */
#define MAX_HELP_PAGES     7

/* The number of palette colors displayed at once */
#define PALETTE_COLORS_PER_PAGE     32

#define FIRST_COLOR_ON_FIRST_PAGE    1
#define LAST_COLOR_ON_FIRST_PAGE    32
#define FIRST_COLOR_ON_SECOND_PAGE  33
#define LAST_COLOR_ON_SECOND_PAGE   64

/* The internal frame rate of various timers */
#define FRAME_RATE        30

/* Information about picture files that can be loaded */
#define MAX_COLLECTIONS 1000
#define MAX_FILES   1000
#define COLLECTION_PATHSPEC "res/pics/*"
#define PIC_FILE_DIR "res/pics"
#define PROGRESS_FILE_DIR "progress"

#define MOUSE_MODE_NEUTRAL             0
#define MOUSE_MODE_DRAW                1
#define MOUSE_MODE_ERASE               2

/* Width and height of all characters from ASCII values 32 to 127 in the
   standard proportional font*/
extern int g_prop_font_width[];
extern int g_prop_font_height;

/* Offsets into the font bitmap for each character (based on the character 
   widths defined in prop_font_width) */
extern int prop_font_offset[];

/* A counter used by the interrupt to update the status screen */
extern volatile unsigned long g_frame_counter;

/* Set to true when the timer triggers, so processing for the new frame
   can begin */
extern volatile int g_next_frame;

/* The elapsed play time for the loaded picture */
extern volatile unsigned int g_elapsed_time;

/* A counter to count down to the next clock update */
extern int g_time_to_update_elapsed;

/* Should the elapsed time be advancing? */
extern int g_game_timer_running;

/* The list of picture categories as stored in a Picture's headers */
extern char *g_categories[];

/* An array of key codes.  Used to prevent keys from retriggering
 * until released */
extern unsigned char g_keypress_lockout[];

/* Image bitmaps used by the game */
extern BITMAP *g_logo;
extern BITMAP *g_title_area;
extern BITMAP *g_title_box;
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
extern BITMAP *g_save_notice;
extern BITMAP *g_load_notice;
extern BITMAP *g_load_dialog;
extern BITMAP *g_overview_box;
extern BITMAP *g_overview_cursor;
extern BITMAP *g_finished_dialog;
extern BITMAP *g_mouse_cursor;
extern BITMAP *g_help_previous;
extern BITMAP *g_help_next;
extern BITMAP *g_help_exit;

/* The parts of the screen to render */
extern RenderComponents g_components;

/* The currently active Picture */
extern Picture *g_picture;

/* The on-disk name of the collection (used to access the collection name) */
extern char g_collection_name[];

/* The on-disk name of the picture file (used to name the progress file) */
extern char g_picture_file_basename[];

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

/* Timer to automatically move from logo to title screen if no key is pressed */
extern int g_title_countdown;

/* Timer between when the image is completed and the screen changes over to
   the replay screen */
extern int g_finished_countdown;

/* How many pixels to add to the replay on every frame */
extern int g_replay_increment;

/* How many pixels from the replay have we rendered so far? */
extern int g_replay_total;

/* Where on screen should we put the top left corner of the replay? */
extern int g_replay_x;
extern int g_replay_y;

/* For smaller replays, this will hold the smallest integer scale that
   allows the replay to cover at least half of the size of the larger 
   of the two image dimensions */
extern int g_replay_scale;

/* Is this the first time we've started the replay loop? */
extern int g_replay_first_time;

/* Title animation controls */
extern TitleAnimation g_title_anim;

/* The Allegro datafile containing image resources */
extern DATAFILE *g_res;

/* The collections to be displayed in the 'Load file' menu */
extern CollectionItem g_collection_items[MAX_FILES];

/* The pictures to be displayed in the 'Load file' menu */
extern PictureItem g_pic_items[MAX_FILES];

/* The actual index of the selected collection in the load file dialog */
extern int g_load_collection_index;

/* The actual index of the selected picture in the load file dialog */
extern int g_load_picture_index;

/* The index of the category shown at the top of the list */
extern int g_load_collection_offset;

/* The index of the picture shown at the top of the list */
extern int g_load_picture_offset;

/* The position (0 - 10) of the highlighted item in the collection list */
extern int g_load_collection_cursor_offset;

/* The position (0 - 10) of the highlighted item in the list */
extern int g_load_cursor_offset;

/* The number of picture collections available to display */
extern int g_num_collections;

/* The number of picture files available to display in the Load File menu */
extern int g_num_picture_files;

/* Which section of the load dialog (collection or image) is currently active */
extern int g_load_section_active;

/* A flag that's set when the 
   load dialog confirms a new file should be loaded */
extern int g_load_new_file;

/* Which help page is visible */
extern int g_help_page;

/* Are mouse click detections being locked out?  This is used as part of the
   process to detect a click.  If the mouse is pressed down and then released,
   the lockout is enabled until the mouse is pressed again */
extern int g_mouse_click_lockout;

/* Flags to tell the renderer whether to highlight these buttons or not */
extern int g_highlight_style_button;
extern int g_highlight_save_button;
extern int g_highlight_load_button;

/* The mouse mode while in the game state.  Can be NEUTRAL (not drawing or erasing),
   DRAW (currently drawing) or ERASE (currently erasing). */
extern int g_game_area_mouse_mode;

/* Mouse position info.  Used as a snapshot for the mouse that can be processed */
extern int g_mouse_x;
extern int g_mouse_y;

extern int g_old_mouse_x;
extern int g_old_mouse_y;

/* A priority flag, used to determine whether the keyboard or mouse has
   priority in terms of moving the cursor or drawing/erasing squares */
extern int g_keyboard_has_priority;

#endif
