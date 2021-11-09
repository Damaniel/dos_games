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

/* The internal frame rate of various timers */
#define FRAME_RATE        30

/* Number of frames to wait after a key is held down before
   it counts as a keypress again */
#define DEBOUNCE_FRAMES    8

BITMAP *g_numbers;
BITMAP *g_bg_lower;
BITMAP *g_bg_right;
BITMAP *g_mainarea;
BITMAP *g_pal_col;
BITMAP *g_draw_cursor;

int g_draw_cursor_x;
int g_draw_cursor_y;

int g_pic_render_x;
int g_pic_render_y;
int g_game_done;

unsigned char g_keypress_lockout[128];

/* A list of areas of the screen to update when calling render_screen() */
typedef struct {
  int render_main_area_squares;  /* The visible part of the picture in the main area*/
  int render_palette_area;       /* The palette columns on the right */
  int render_palette;            /* The numbers/colors in the palette on the right */
  int render_ui_components;      /* All static UI elements */
  int render_overview_display;   /* The overview/progress area in the top right */
  int render_status_text;        /* The time/progress text at the bottom */
  int render_draw_cursor;             /* Update the cursor */
  int render_all;                /* Render everything - convenience function */
} RenderComponents;

typedef struct {
  char pal_entry;                /* The color this square should be */
  char fill_value;               /* 0 = not filled in, x = value actually filled in */
  int  order;                    /* The order in which this square was *correctly* filled in */
} ColorSquare;

typedef struct {
  int x;                         /* The x and y position of the Nth square filled in */
  int y;                         /* correctly by the player */
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

/* From input.c */
int process_input(int state);

/* From dampbn.c */
Picture *load_picture(char *filename);
void render_main_area_squares(BITMAP *dest, int x_off, int y_off);
void render_screen(BITMAP *dest, RenderComponents c);
int load_graphics(void);
void destroy_graphics(void);

#endif
