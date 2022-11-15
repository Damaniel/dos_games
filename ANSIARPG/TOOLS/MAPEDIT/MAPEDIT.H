#ifndef __MAPEDIT_H__
#define __MAPEDIT_H__

#include "textdraw.h"
#include <bios.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define NUM_PALETTE_ENTRIES     16
#define MAP_WIDTH              150
#define MAP_HEIGHT             150

// The position and size of the drawable map area
#define MAP_AREA_X               1
#define MAP_AREA_Y               2

#define MAP_AREA_WIDTH          61
#define MAP_AREA_HEIGHT         22

#define PALETTE_AREA_X          63
#define PALETTE_AREA_Y           2

#define NUM_EXITS                5

typedef struct {
    char cursor_x;          // position of the cursor on screen
    char cursor_y;          
    char map_x;             // position of the map at the upper left corner of
    char map_y;             // the draw area
    char palette_entry;
    char exit_entry;    
} AppConfig;

typedef struct {
    char background_attr;
    char menu_attr;
} UIConfig;

typedef struct {
    char render_background;
    char render_menu;
    char render_palette;
    char render_palette_text;
    char render_palette_active;
    char render_exits_text;
    char render_exit_active;
    char render_map_area;
} RenderComponents;

typedef struct {
    unsigned char id;
    char name[9];
    unsigned char glyph;
    unsigned char fg;
    unsigned char bg;
    unsigned char flags1;
    unsigned char flags2;
} PaletteEntry;

typedef struct {
    unsigned char is_set;
    unsigned char target_room;
    unsigned char x_pos;
    unsigned char y_pos;
} Exit;


extern UIConfig g_ui_config;
extern AppConfig g_app_config;

extern RenderComponents g_render_components;
extern PaletteEntry g_map_palette[];
extern Exit g_exit_list[];

void wait_for_key(void);
void set_all_render_components(void);
void clear_render_components(void);
void initialize_attributes(void);
void render(void);
void set_map_at(int x, int y, unsigned char palette_entry);

#endif
