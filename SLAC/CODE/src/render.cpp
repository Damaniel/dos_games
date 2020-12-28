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

void Render::render_world_at(int x, int y) {
}