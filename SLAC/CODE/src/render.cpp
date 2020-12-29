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

void Render::render_statics(BITMAP *destination, int x, int y) {
	
}

// Renders all non-static dungeon elements.  This includes the player and all
// enemies.  
void Render::render_actors(BITMAP *destination, int x, int y) {
	BITMAP *bpc = (BITMAP *)g_game_data[DAMRL_PLAYER_SPRITES].dat;
	// For now, just draw the player
	masked_blit(bpc, destination, PLAYER_TILE_OFFSET * TILE_PIXEL_WIDTH, 0, 
	            PLAYER_PLAY_AREA_X * TILE_PIXEL_WIDTH, PLAYER_PLAY_AREA_Y * TILE_PIXEL_HEIGHT,
		        TILE_PIXEL_WIDTH, TILE_PIXEL_HEIGHT);
}

void Render::render_world_at(BITMAP *destination, Maze m, int x, int y) {

	// Render the world with the 0,0 tile position equal to (x,y).  
	// Negative values for x and y are allowed, as are values outside of the positive
	// end of the range - tiles just won't be drawn for invalid locations
	
	// Also note that the code that draws the floor (that is, where isCarved == true),
	// checks tiles to the left and above it when deciding what to draw.  There isn't 
	// any check done to see if those tiles are valid, but the default Maze class will 
	// always have a valid tile to the left and above any carved tile, so this shouldn't
	// cause a problem.  If weird crashes happen, try looking here.
	for (int i=0; i<PLAY_AREA_TILE_WIDTH; i++) {
		for (int j=0; j<PLAY_AREA_TILE_HEIGHT; j++) {
			int tilex = x + i;
			int tiley = y + j;
			int tileToUse;
			bool carvedLeft = m.isCarved(tilex -1, tiley);
			bool carvedUp = m.isCarved(tilex, tiley - 1);
			
			if(tilex >=0 && tiley >=0 && tilex < m.getWidth() && tiley < m.getHeight()) {
				int stairs = m.stairsHere(tilex, tiley);
				if (stairs == STAIRS_UP) {
					blit((BITMAP *)g_game_data[DAMRL_MAZE_BASE_TILES_1].dat, 
					     destination,
					     TILE_UP_STAIRS * TILE_PIXEL_WIDTH, 
						 0, 
						 i * TILE_PIXEL_WIDTH,
						 j * TILE_PIXEL_HEIGHT,
						 TILE_PIXEL_WIDTH,
						 TILE_PIXEL_HEIGHT);					
				}
				else if (stairs == STAIRS_DOWN) {
					blit((BITMAP *)g_game_data[DAMRL_MAZE_BASE_TILES_1].dat, 
					     destination,
					     TILE_DOWN_STAIRS * TILE_PIXEL_WIDTH, 
						 0, 
						 i * TILE_PIXEL_WIDTH,
						 j * TILE_PIXEL_HEIGHT,
						 TILE_PIXEL_WIDTH,
						 TILE_PIXEL_HEIGHT);						
				}
				else if (m.isCarved(tilex, tiley) == true) {
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
					blit((BITMAP *)g_game_data[DAMRL_MAZE_BASE_TILES_1].dat, 
					     destination,
					     tileToUse * TILE_PIXEL_WIDTH, 
						 0, 
						 i * TILE_PIXEL_WIDTH,
						 j * TILE_PIXEL_HEIGHT,
						 TILE_PIXEL_WIDTH,
						 TILE_PIXEL_HEIGHT);
				} else {
					blit((BITMAP *)g_game_data[DAMRL_MAZE_BASE_TILES_1].dat, 
					     destination,
					     TILE_WALL * TILE_PIXEL_WIDTH, 
						 0, 
						 i * TILE_PIXEL_WIDTH,
						 j * TILE_PIXEL_HEIGHT,
						 TILE_PIXEL_WIDTH,
						 TILE_PIXEL_HEIGHT);
				}		
			}
		}
	}
	
	render_statics(destination, x, y);
	render_actors(destination, x, y);
}