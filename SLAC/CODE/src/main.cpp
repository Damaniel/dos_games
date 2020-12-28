#include <allegro.h>
#include <stdio.h>
#include "render.h"
#include "globals.h"
#include "maze.h"

DATAFILE *g_game_data;

int load_resources(void) {
	
	g_game_data = load_datafile("game.dat");
	if(g_game_data == NULL) {
		printf("Unable to open game data file.  Is it in the right place?\n");
		return 1;
	}		
	return 0;
}

/* Perform any initializtion tasks that need to be done with data in the datafile */
void init_resources(Render r) {
	r.generate_prop_font_offsets();		
	r.copy_data_to_offscreen_vram();	
	set_palette((PALETTE)g_game_data[DAMRL_DB16].dat);	
}

void unload_resources(void) {
	unload_datafile(g_game_data);
}

int main(void) {

	int x, y, key;
	bool done, update_display;
	
	srand(time(NULL));
	
	allegro_init();
	install_timer();
	install_keyboard();

	set_gfx_mode(GFX_MODEX, 320, 240, 320, 720);
	clear(screen);
	
	Render r = Render();
	
	int result = load_resources();
	if(result != 0) {
		printf("Failure while loading resources!\n");
		return 1;
	}

	init_resources(r);

	Maze m = Maze(64, 64);
	m.generate();
	
	x = 0;
	y = 0;
	done = false;
	update_display = true;
	
	BITMAP *memBmp = create_bitmap(240, 208);
	
	r.render_status_base(screen);
	r.render_text_base(screen, false);
	
	while (done == false) {
		if (update_display == true) {
			r.render_world_at(memBmp, m, x, y);
			blit(memBmp, screen, 0, 0, 0, 0, 240, 208);
			update_display = false;
		}
		
		key = (readkey() >> 8);
		if (key == KEY_ESC) {
			done = true;
		}
		if (key == KEY_LEFT) {
			x = x -1;
			update_display = true;
		}
		if (key == KEY_RIGHT) {
			x = x + 1;
			update_display = true;
		}
		if (key == KEY_UP) {
			y = y - 1;
			update_display = true;
		}
		if (key == KEY_DOWN) {
			y = y + 1;
			update_display = true;
		}
		

	}
		
	
	unload_resources();
	set_gfx_mode(GFX_TEXT, 80, 25, 0, 0);

	return 0;
}
