#ifndef __RENDER_H__
#define __RENDER_H__

#include "textdraw.h"
#include "maze.h"
#include "player.h"

#define MAZE_AREA_WIDTH         60
#define MAZE_AREA_HEIGHT        33

extern char g_lines;

extern Rect g_menu_bar;
extern Rect g_main_window;
extern Rect g_play_area;
extern Rect g_status_area;
extern Rect g_extra_area;
extern Rect g_text_box_window;
extern Rect g_text_box_text_area;

extern void set_extents(void);
extern void render_main_ui(void);
extern void render_maze(void);
extern void render_status(void);
extern void render_logs(void);
extern void render_play_area(void);

#endif