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
void init_resources(void) {
	generate_prop_font_offsets();		
	copy_data_to_offscreen_vram();	
	set_palette((PALETTE)g_game_data[DAMRL_DB16].dat);	
}

void unload_resources(void) {
	unload_datafile(g_game_data);
}

int main(void) {

	int i, j; 
	int result;
	
	srand(time(NULL));
	
	allegro_init();
	install_timer();
	install_keyboard();

	set_gfx_mode(GFX_MODEX, 320, 240, 320, 720);
	clear(screen);
	
	result = load_resources();
	if(result != 0) {
		printf("Failure while loading resources!\n");
		return 1;
	}

	init_resources();

	render_map();
		
	Maze m = Maze(64, 64);
	m.generate();
		
	for (i = 0; i < m.getHeight(); i++) {
		for (j = 0; j < m.getWidth(); j++) {
			if (m.isCarved(j, i) == false) {
				putpixel(screen, 7+j*2, 16+i*2, 15);
				putpixel(screen, 7+j*2+1, 16+i*2, 15);
				putpixel(screen, 7+j*2, 16+i*2+1, 15);
				putpixel(screen, 7+j*2+1, 16+i*2+1, 15);
			}
		}
	}

	while(!keypressed()) {
	}
	
	unload_resources();
	set_gfx_mode(GFX_TEXT, 80, 25, 0, 0);

	return 0;
}
