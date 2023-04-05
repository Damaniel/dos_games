#ifndef __GLOBALS_H__
#define __GLOBALS_H__

#define MAP_SCREEN_WIDTH        14
#define MAP_SCREEN_HEIGHT       12

// Graphics positions
#define MAP_TILE_AREA_X          4
#define MAP_TILE_AREA_Y          4

#define UI_MAP_WINDOW_X          0
#define UI_MAP_WINDOW_Y          0

#define MAP_WIDTH   14
#define MAP_HEIGHT  10

// The number of pages (14x10 tile arrangements) that make up the entire map.
// The map is MAP_WIDTH_PAGES x MAP_HEIGHT_PAGES in size, and each page
// is defined by a single .MAP file containing the tile data
#define MAP_WIDTH_PAGES         2
#define MAP_HEIGHT_PAGES         1

extern MapSquare g_map_page[MAP_WIDTH][MAP_HEIGHT];
extern MapLocation g_map_location;

extern RenderComponents g_render_components;
extern BITMAP *g_tiles;
extern BITMAP *g_main_ui_bg;
extern PALETTE g_pal;

#endif
