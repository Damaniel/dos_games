#ifndef __RENDER_H__
#define __RENDER_H__

#include "maze.h"

/* Number of characters defined in each instance of a font */
#define FONT_ENTRIES       96

/* A copy of the map bitmap is stored at this location in video memory. */
#define MAP_VMEM_X	0
#define MAP_VMEM_Y	280

/* The size of the map bitmap in offscreen memory */
#define MAP_VMEM_WIDTH   144
#define MAP_VMEM_HEIGHT  160

/* The size of a dungeon tile in pixels */
#define TILE_PIXEL_WIDTH		  16
#define TILE_PIXEL_HEIGHT	      16

/* The size of the visible play area (in tiles)
 * PLAY_AREA_TILE_HEIGHT_EXT measures the height in tiles when the extended message
 * log is displayed. */
#define PLAY_AREA_TILE_WIDTH   		15
#define PLAY_AREA_TILE_HEIGHT  		13
#define PLAY_AREA_TILE_HEIGHT_EXT	10

#define STATUS_AREA_X			   240
#define STATUS_AREA_Y				 0

#define TEXT_AREA_STD_X			     0
#define TEXT_AREA_STD_Y			   208

#define TEXT_AREA_EXT_X              0
#define TEXT_AREA_EXT_Y			   160

// The offset (in tiles) into the player graphic where the player is.  This will eventually
// include different characters, but this one is a placeholder.
#define PLAYER_TILE_OFFSET			 9

// The offset (in tiles) into the play area where the player graphic is placed.  The player
// will always be centered in the play area.
#define PLAYER_PLAY_AREA_X			 7
#define PLAYER_PLAY_AREA_Y			 6

/* Each font has 5 different versions in different colors.  These are used to grab the correct
   color font from the bitmaps */
enum {
	FONT_YELLOW,
	FONT_BLUE,
	FONT_GREEN,
	FONT_RED,
	FONT_GRAY
};

/* The base tiles that make up the fixed portions of a dungeon level */
enum {
	TILE_DARK,
	TILE_BORDER_WALL,
	TILE_WALL,
	TILE_FLOOR,
	TILE_FLOOR_TOP_HIGHLIGHT,
	TILE_FLOOR_LEFT_HIGHLIGHT,
	TILE_FLOOR_BOTH_HIGHLIGHT,
	TILE_UP_STAIRS,
	TILE_DOWN_STAIRS,
	TILE_DOOR
};
	
/* Width and height of all characters from ASCII values 32 to 127 in the standard 
   proportional font */
const int prop_font_width[FONT_ENTRIES] = {
	6, 2, 6, 7, 6, 6, 7, 2, 4, 4, 5, 5, 2, 6, 2, 6, 
	7, 4, 7, 7, 7, 7, 7, 7, 7, 7, 2, 2, 5, 6, 5, 6, 
	8, 8, 7, 7, 7, 7, 7, 7, 8, 4, 7, 8, 7, 8, 8, 7, 
	7, 7, 8, 7, 6, 8, 8, 8, 8, 8, 6, 4, 6, 4, 5, 8, 
	3, 7, 7, 6, 7, 6, 5, 7, 8, 4, 6, 7, 4, 8, 8, 6, 
	7, 7, 7, 6, 4, 7, 8, 7, 6, 6, 6, 4, 2, 4, 7, 7  
};		
const int prop_font_height = 8;

/* Width and height of all characters in the standard fixed font */
const int fixed_font_width = 8;
const int fixed_font_height = 8;

/* The Render class itself */
class Render {
	private:
		/* This is filled in by a function based on prop_font_width */
		int prop_font_offset[FONT_ENTRIES];
		
		void render_statics(BITMAP *destination, int x, int y);
		void render_actors(BITMAP *destination, int x, int y);
	
	public:
		Render();
		void generate_prop_font_offsets(void);
		void render_prop_text(BITMAP *destination, char *text, int x_pos, int y_pos, int font_idx);
		void render_fixed_text(BITMAP *destination, char *text, int x_pos, int y_pos, int font_idx);
		void copy_data_to_offscreen_vram(void);
	
		/* Functions to draw various screen components */	
		void render_map_to_screen(void);
		void render_world_at(BITMAP *destination, Maze m, int x, int y);
		void render_text_base(BITMAP *destination, bool extended);
		void render_status_base(BITMAP *destination);
};
 
#endif	
	
	
	
	