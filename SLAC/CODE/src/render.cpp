//==========================================================================================
//   Secret Legacy of the Ancient Caves (SLAC)
//
//   Copyright (c) 2020-2021 Shaun Brandt / Holy Meatgoat Software
//
//   Permission is hereby granted, free of charge, to any person obtaining a copy
//   of this software and associated documentation files (the "Software"), to deal
//   in the Software without restriction, including without limitation the rights
//   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//   copies of the Software, and to permit persons to whom the Software is
//   furnished to do so, subject to the following conditions:
//
//   The above copyright notice and this permission notice shall be included in all
//   copies or substantial portions of the Software.
//
//   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//   SOFTWARE.
//==========================================================================================
#include <allegro.h>
#include "globals.h"
#include "render.h"


//==================================================================================
// Constructors
//==================================================================================
Render::Render() {

}

//==================================================================================
// Private methods
//==================================================================================

//----------------------------------------------------------------------------------
// render_actors
//
// Renders all non-static dungeon elements.  This includes the player and all
// enemies.  
//
// Notes:
//   maze_x and maze_y represent the maze tile position in the upper left corner of 
//   the play area
//----------------------------------------------------------------------------------
void Render::render_actors(BITMAP *destination, int maze_x, int maze_y) {
	BITMAP *bpc = (BITMAP *)g_game_data[DAMRL_PLAYER_SPRITES].dat;
	// For now, just draw the player
	masked_blit(bpc, destination, PLAYER_TILE_OFFSET * TILE_PIXEL_WIDTH, 0, 
	            PLAYER_PLAY_AREA_X * TILE_PIXEL_WIDTH, PLAYER_PLAY_AREA_Y * TILE_PIXEL_HEIGHT,
		        TILE_PIXEL_WIDTH, TILE_PIXEL_HEIGHT);
}

//----------------------------------------------------------------------------------
// render_base_tile
//
// Draws the specified base tile (that is, wall, floor, or darkness) at the tile
// location (x,y) relative to the top of the play area
//----------------------------------------------------------------------------------
void Render::render_base_tile(BITMAP *destination, int tileId, int x, int y) {
	blit((BITMAP *)g_game_data[DAMRL_MAZE_BASE_TILES_1].dat, 
	     destination,
	     tileId * TILE_PIXEL_WIDTH, 
		 0, 
		 x * TILE_PIXEL_WIDTH,
		 y * TILE_PIXEL_HEIGHT,
		 TILE_PIXEL_WIDTH,
		 TILE_PIXEL_HEIGHT);	
}

//----------------------------------------------------------------------------------
// render_statics
//
// Renders all non-actor, non-architectural dungeon elements.  This includes 
// items, traps, money, doors, and so on.
//
// Notes:
//   maze_x and maze_y represent the maze tile position in the upper left corner of 
//   the play area
//----------------------------------------------------------------------------------
void Render::render_statics(BITMAP *destination, int maze_x, int maze_y) {
	
}

//==================================================================================
// Public methods
//==================================================================================

//----------------------------------------------------------------------------------
// copy_data_to_offscreen_vram
//
// Takes any bitmap data that we plan to stuff in unused video RAM and load it
// into a free spot
//
// Notes:
//   The only bitmap data being put into video memory right now is the map dialog.
//   The map will be constructed as the player explores, and by doing this as 
//   exploration occurs, there's no need to generate it at the time the map is
//   displayed (which would be a very slow operation).
//----------------------------------------------------------------------------------
void Render::copy_data_to_offscreen_vram(void) {
	BITMAP *b = (BITMAP *)g_game_data[DAMRL_MAIN_MAP].dat;	
	blit(b, screen, 0, 0, MAP_VMEM_X, MAP_VMEM_Y, MAP_VMEM_WIDTH, MAP_VMEM_HEIGHT);
}

//----------------------------------------------------------------------------------
// generate_prop_font_offsets
//
// Calculates the offsets into the proportional font bitmap based on the width of 
// each character.  By precomputing this, text glyphs can be rendered more quickly.
//----------------------------------------------------------------------------------
void Render::generate_prop_font_offsets(void) {
	int offset = 0;
	int i;
	
	for (i=0; i < FONT_ENTRIES; i++) {
		prop_font_offset[i] = offset;
		offset += prop_font_width[i];
	}
}

//----------------------------------------------------------------------------------
// render_fixed_text
//
// Writes a string to the screen in the specified location using the specified color
// using a fixed width font.
//
// Notes: 
//   The font bitmap contains all characters in the ASCII range 32-128.  To find the
//   correct offset into the bitmap, the ASCII value needs to be subtracted by 32.
//----------------------------------------------------------------------------------
void Render::render_fixed_text(BITMAP *destination, char *text, int x_pos, int y_pos, int font_idx) {

	int x = x_pos;
	int y = y_pos;
	int offset;
	char *cur = text;
	
	// This isn't a library, so we'll assume strings are terminated.  That should end well...
	while(*cur != 0) {
		offset = (*cur++) - 32;
		blit((BITMAP *)g_game_data[DAMRL_FIXED_FONT].dat, destination, offset * fixed_font_width, 
		     font_idx * fixed_font_height, x, y, fixed_font_width, fixed_font_height);
		x += 8;
	}
}

//----------------------------------------------------------------------------------
// render_map_to_screen
//
// Displays the map dialog in the play area.
//
// Notes: 
//   The map dialog is stored in otherwise unused VGA memory.
//----------------------------------------------------------------------------------
void Render::render_map_to_screen(void) {
	blit(screen, screen, MAP_VMEM_X, MAP_VMEM_Y, 0, 0, MAP_VMEM_WIDTH, MAP_VMEM_HEIGHT);
}

//----------------------------------------------------------------------------------
// render_prop_text
//
// Writes a string to the screen in the specified location using the specified color
// using a proportional font.
//
// Notes: 
//   The font bitmap contains all characters in the ASCII range 32-128.  To find the
//   correct offset into the bitmap, the ASCII value needs to be subtracted by 32.
//----------------------------------------------------------------------------------
void Render::render_prop_text(BITMAP *destination, char *text, int x_pos, int y_pos, int font_idx) {
	int x = x_pos;
	int y = y_pos;
	int offset;
	char *cur = text;
	
	while (*cur != 0) {
		offset = (*cur++) - 32;
		blit((BITMAP *)g_game_data[DAMRL_PROP_FONT].dat, destination, prop_font_offset[offset],
		     font_idx * prop_font_height, x, y, prop_font_width[offset], prop_font_height);
		x += prop_font_width[offset] + 1;
	}
}

//----------------------------------------------------------------------------------
// render_status_base
//
// Draws the status background graphic to the screen.
//----------------------------------------------------------------------------------
void Render::render_status_base(BITMAP *destination) {
	BITMAP *b = (BITMAP *)g_game_data[DAMRL_MAIN_STATUS].dat;
	blit(b, destination, 0, 0, STATUS_AREA_X, STATUS_AREA_Y, b->w, b->h);
}

//----------------------------------------------------------------------------------
// render_text_base
//
// Draws the text area graphic to the screen.
//
// Note:
//   If extended is true, the larger extended text dialog will be used.
//----------------------------------------------------------------------------------
void Render::render_text_base(BITMAP *destination, bool extended) {
	BITMAP *bstd = (BITMAP *)g_game_data[DAMRL_MAIN_TEXT].dat;
	BITMAP *bext = (BITMAP *)g_game_data[DAMRL_MAIN_TEXT_EXT].dat;
	
	if(extended == true) {
		blit(bext, destination, 0, 0, TEXT_AREA_EXT_X, TEXT_AREA_EXT_Y, bext->w, bext->h);
	}
	else {
		blit(bstd, destination, 0, 0, TEXT_AREA_STD_X, TEXT_AREA_STD_Y, bstd->w, bstd->h);		
	}
}

//----------------------------------------------------------------------------------
// render_world_at
//
// Render the world with the tile at screen position (0, 0) position equal to (maze_x, maze_y).  
//
// Notes:
//   Negative values for maze_x and maze_y are allowed, as are values outside of the 
//   positive end of the range - maze tiles just won't be drawn for invalid locations	
//   Also note that the code that draws the floor (that is, where isCarved == true),
//   checks tiles to the left and above it when deciding what to draw.  There isn't 
//   any check done to see if those tiles are valid, but the default Maze class will 
//   always have a valid tile to the left and above any carved tile, so this shouldn't
//   cause a problem.  If weird crashes happen, try looking here.
//----------------------------------------------------------------------------------
void Render::render_world_at(BITMAP *destination, Maze m, int maze_x, int maze_y) {
	for (int screen_x = 0; screen_x < PLAY_AREA_TILE_WIDTH; screen_x++) {
		for (int screen_y = 0; screen_y < PLAY_AREA_TILE_HEIGHT; screen_y++) {
			int tile_to_render_x = maze_x + screen_x;
			int tile_to_render_y = maze_y + screen_y;
			int tileToUse;
			bool carvedLeft = m.isCarved(tile_to_render_x -1, tile_to_render_y);
			bool carvedUp = m.isCarved(tile_to_render_x, tile_to_render_y - 1);
			
			if(tile_to_render_x >=0 && tile_to_render_y >=0 && tile_to_render_x < m.getWidth() && tile_to_render_y < m.getHeight()) {
				int stairs = m.stairsHere(tile_to_render_x, tile_to_render_y);				
				// Before checking any other status, draw darkness if the square isn't lit
				if (m.isSquareLit(tile_to_render_x, tile_to_render_y) == false) { 
					// If the square has previously been seen and isn't carved, draw a darker wall
					if (m.isCarved(tile_to_render_x, tile_to_render_y) == false && m.wasSeen(tile_to_render_x, tile_to_render_y) == true) {
						render_base_tile(destination, TILE_DARKER_WALL, screen_x, screen_y);
					} else {
						// Otherwise, draw darkness
						render_base_tile(destination, TILE_DARK, screen_x, screen_y);
					}
				}
				// Render stairs if present
				else if (stairs == STAIRS_UP) {
					render_base_tile(destination, TILE_UP_STAIRS, screen_x, screen_y);
				}
				else if (stairs == STAIRS_DOWN) {
					render_base_tile(destination, TILE_DOWN_STAIRS, screen_x, screen_y);
				}
				// Render floor if present.  There are 4 different floor tiles - one with no
				// highlighting and 3 with different types of highlighting
				// If the location is a wall, render that instead.
				else if (m.isCarved(tile_to_render_x, tile_to_render_y) == true) {
					if (carvedLeft == false && carvedUp == true) {
						tileToUse = TILE_FLOOR_LEFT_HIGHLIGHT;
					}
					else if (carvedLeft == true && carvedUp == false) {
						tileToUse = TILE_FLOOR_TOP_HIGHLIGHT;
					}
					else if (carvedLeft == false && carvedUp == false) {
						tileToUse = TILE_FLOOR_BOTH_HIGHLIGHT;
					}
					else {
						tileToUse = TILE_FLOOR;
					}
					render_base_tile(destination, tileToUse, screen_x, screen_y);
				} else {
					render_base_tile(destination, TILE_WALL, screen_x, screen_y);
				}		
			} else {
				// Draw an empty space since it's outside of the map
				render_base_tile(destination, TILE_DARK, screen_x, screen_y);
			}						 
		}
	}
	
	// Render the other world components
	render_statics(destination, maze_x, maze_y);
	render_actors(destination, maze_x, maze_y);
}

//----------------------------------------------------------------------------------
// render_world_at_player
//
// Equivalent to render_world_at, but renders using the player's location as the
// center of the play area rather than (0, 0) as the top left of the play area.
// This way, all rendering is done relative to the player, who sits in a fixed
// location on the screen.
//----------------------------------------------------------------------------------
void Render::render_world_at_player(BITMAP *destination, Maze m, int maze_x, int maze_y) {
	// Render the world with the tile at the player's position (7,6) equal to (maze_x, maze_y)
	render_world_at(destination, m, maze_x - PLAYER_PLAY_AREA_X, maze_y - PLAYER_PLAY_AREA_Y);
}