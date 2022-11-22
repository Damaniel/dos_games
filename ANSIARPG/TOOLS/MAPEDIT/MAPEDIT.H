#ifndef __MAPEDIT_H__
#define __MAPEDIT_H__

#include "textdraw.h"
#include "render.h"
#include "input.h"

#include <bios.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define MAP_WIDTH              150
#define MAP_HEIGHT             150

// The position and size of the drawable map area
#define MAP_AREA_X               1
#define MAP_AREA_Y               2

#define MAP_AREA_WIDTH          61
#define MAP_AREA_HEIGHT         22

// The minimum and maximum positions of the map cursor on screen
#define MIN_SCREEN_CURSOR_X            MAP_AREA_X
#define MIN_SCREEN_CURSOR_Y            MAP_AREA_Y
#define MAX_SCREEN_CURSOR_X            (MAP_AREA_X + MAP_AREA_WIDTH - 1)
#define MAX_SCREEN_CURSOR_Y            (MAP_AREA_Y + MAP_AREA_HEIGHT - 1)

// The minimum and maximum positions of the top left of the map on the screen
// (used to ensure that the map never displays area outside of the map array)
#define MIN_SCREEN_MAP_X                0
#define MIN_SCREEN_MAP_Y                0
#define MAX_SCREEN_MAP_X                (MAP_WIDTH - MAP_AREA_WIDTH)
#define MAX_SCREEN_MAP_Y                (MAP_HEIGHT - MAP_AREA_HEIGHT)

#define PALETTE_AREA_X          63
#define PALETTE_AREA_Y           2

// Palette 0 can't be redefined.  
#define NUM_PALETTE_ENTRIES     16
#define MIN_PALETTE_ENTRY        1
#define MAX_PALETTE_ENTRY       NUM_PALETTE_ENTRIES - 1

#define NUM_EXITS                5

#define PALETTE_EDIT_X          20
#define PALETTE_EDIT_Y           6

#define PALETTE_EDIT_WIDTH      24
#define PALETTE_EDIT_HEIGHT     14

#define PALETTE_ITEM_NAME_LENGTH    8

// Flag 1 bits
// bit 0     solid (1) / passable (0)
#define FLAG_SOLID_MASK               0x01
#define FLAG_PASSABLE                 0x00
#define FLAG_SOLID                    0x01
// bits 1-3  floor damage types (000 - none, 001 - low, 010 - medium, 011 - high, 100 - OHKO)
#define FLAG_DAMAGE_MASK              0x0E
#define FLAG_DAMAGE_NONE              0x00
#define FLAG_DAMAGE_LOW               0x01
#define FLAG_DAMAGE_MEDIUM            0x02
#define FLAG_DAMAGE_HIGH              0x03
#define FLAG_DAMAGE_OHKO              0x04

typedef struct {
    unsigned char cursor_x;          // position of the cursor in the map area
    unsigned char cursor_y;          
    unsigned char old_cursor_x;      // The previous position of the cursor.  Used to
    unsigned char old_cursor_y;      // remove the old cursor when moving
    unsigned char map_x;             // position of the map at the upper left corner of
    unsigned char map_y;             // the draw area
    unsigned char palette_entry;
    unsigned char exit_entry;    
    unsigned char quit;              // Quit the program?
} AppConfig;

typedef struct {
    char background_attr;
    char menu_attr;
    char highlight_attr;
} UIConfig;

typedef enum {
    PI_NAME,
    PI_FOREGROUND,
    PI_BACKGROUND,
    PI_CHARACTER,
    PI_SOLID,
    PI_DAMAGE,
    PI_OK,
    PI_CANCEL
} PaletteMenuItem;

typedef enum {
    DT_NONE,
    DT_LOW,
    DT_MEDIUM,
    DT_HIGH,
    DT_OHKO
} DamageType;

typedef struct {
    PaletteMenuItem active_item;
    unsigned char name_idx;
    char name[9];
    unsigned char foreground;
    unsigned char background;
    unsigned char character;
    unsigned char solid;
    DamageType damage_type;
} PaletteMenuConfig;

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

typedef enum {
    MAIN_SCREEN,
    EXIT_EDIT,
    PALETTE_EDIT
} State;

extern State g_state;

extern UIConfig g_ui_config;
extern AppConfig g_app_config;
extern PaletteMenuConfig g_palette_menu_config;

extern PaletteEntry g_map_palette[];
extern Exit g_exit_list[];
extern unsigned char g_map[][MAP_HEIGHT];

void set_palette_flags(int index, char solid_value, char damage_value);
char get_palette_damage_value(int index);
char get_palette_solid_value(int index);

void initialize_attributes(void);
void initialize_app_defaults(void);
void initialize_palette_menu_defaults(void);
void render(void);
void set_map_at(int x, int y, unsigned char palette_entry);
char get_map_at(int x, int y);

#endif
