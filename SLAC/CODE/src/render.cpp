#include <allegro.h>
#include "render.h"
#include "globals.h"

Render::Render() {

}

void Render::copy_data_to_offscreen_vram(void) {
	BITMAP *b = (BITMAP *)g_game_data[DAMRL_MAIN_MAP].dat;	
	blit(b, screen, 0, 0, MAP_VMEM_X, MAP_VMEM_Y, MAP_VMEM_WIDTH, MAP_VMEM_HEIGHT);
}

void Render::render_map_to_screen(void) {
	blit(screen, screen, MAP_VMEM_X, MAP_VMEM_Y, 0, 0, MAP_VMEM_WIDTH, MAP_VMEM_HEIGHT);
}

void Render::generate_prop_font_offsets(void) {
	int offset = 0;
	int i;
	
	for (i=0; i < FONT_ENTRIES; i++) {
		prop_font_offset[i] = offset;
		offset += prop_font_width[i];
	}
}

void Render::render_prop_text(BITMAP *destination, char *text, int x_pos, int y_pos, int font_idx) {
	// Note: the font bitmap contains all characters in the ASCII range 32-128.  To find the
	// correct offset into the bitmap, the ASCII value needs to be subtracted by 32.	
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

void Render::render_fixed_text(BITMAP *destination, char *text, int x_pos, int y_pos, int font_idx) {
	// Note: the font bitmap contains all characters in the ASCII range 32-128.  To find the
	// correct offset into the bitmap, the ASCII value needs to be subtracted by 32.
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

void Render::render_status_base(BITMAP *destination) {
	BITMAP *b = (BITMAP *)g_game_data[DAMRL_MAIN_STATUS].dat;
	blit(b, destination, 0, 0, STATUS_AREA_X, STATUS_AREA_Y, b->w, b->h);
}

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

void Render::render_statics(BITMAP *destination, int maze_x, int maze_y) {
	
}

// Renders all non-static dungeon elements.  This includes the player and all
// enemies.  
void Render::render_actors(BITMAP *destination, int maze_x, int maze_y) {
	BITMAP *bpc = (BITMAP *)g_game_data[DAMRL_PLAYER_SPRITES].dat;
	// For now, just draw the player
	masked_blit(bpc, destination, PLAYER_TILE_OFFSET * TILE_PIXEL_WIDTH, 0, 
	            PLAYER_PLAY_AREA_X * TILE_PIXEL_WIDTH, PLAYER_PLAY_AREA_Y * TILE_PIXEL_HEIGHT,
		        TILE_PIXEL_WIDTH, TILE_PIXEL_HEIGHT);
}

void Render::render_world_at_player(BITMAP *destination, Maze m, int maze_x, int maze_y) {
	// Render the world with the tile at the player's position (7,6) equal to (maze_x, maze_y)
	render_world_at(destination, m, maze_x - PLAYER_PLAY_AREA_X, maze_y - PLAYER_PLAY_AREA_Y);
}

void Render::render_world_at(BITMAP *destination, Maze m, int maze_x, int maze_y) {

	// Render the world with the tile at screen position (0, 0) position equal to (maze_x, maze_y).  
	// Negative values for maze_x and maze_y are allowed, as are values outside of the positive
	// end of the range - maze tiles just won't be drawn for invalid locations
	
	// Also note that the code that draws the floor (that is, where isCarved == true),
	// checks tiles to the left and above it when deciding what to draw.  There isn't 
	// any check done to see if those tiles are valid, but the default Maze class will 
	// always have a valid tile to the left and above any carved tile, so this shouldn't
	// cause a problem.  If weird crashes happen, try looking here.
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
				if (m.isLit(tile_to_render_x, tile_to_render_y) == false ) {
					render_base_tile(destination, TILE_DARK, screen_x, screen_y);
				}
				else if (stairs == STAIRS_UP) {
					render_base_tile(destination, TILE_UP_STAIRS, screen_x, screen_y);
				}
				else if (stairs == STAIRS_DOWN) {
					render_base_tile(destination, TILE_DOWN_STAIRS, screen_x, screen_y);
				}
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
	
	render_statics(destination, maze_x, maze_y);
	render_actors(destination, maze_x, maze_y);
}

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
