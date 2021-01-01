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
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <allegro.h>

// Game-specific include files
#include "player.h"
#include "maze.h"
#include "render.h"
#include "globals.h"

// Globals used to hold game state.  External defines are in globals.h
DATAFILE *g_game_data;
BITMAP   *g_back_buffer;
Player 	  g_player;

//----------------------------------------------------------------------------------
// load_resources
//
// Loads and processes all game resources.  Most are stored in the global 
// datafile, so that's all that gets loaded right now.
//----------------------------------------------------------------------------------
int load_resources(void) {
	
	g_game_data = load_datafile("game.dat");
	if(g_game_data == NULL) {
		printf("Unable to open game data file.  Is it in the right place?\n");
		return 1;
	}		
	return 0;
}

//----------------------------------------------------------------------------------
// init_resources
//
// Perform any initializtion tasks that need to be done with game data.
//----------------------------------------------------------------------------------
void init_resources(Render r) {
	r.generate_prop_font_offsets();		
	r.copy_data_to_offscreen_vram();	
	set_palette((PALETTE)g_game_data[DAMRL_DB16].dat);		
	g_back_buffer = create_bitmap(240, 208);
}

//----------------------------------------------------------------------------------
// unload_resources
//
// Free up any game related resources.
//----------------------------------------------------------------------------------
void unload_resources(void) {
	unload_datafile(g_game_data);
}

//----------------------------------------------------------------------------------
// MAIN
//
// Notes:
//   Most of the code in here is just testing stuff.  There's no official game
//   loop of any kind yet, and stuff in here is subject to change and/or removal.
//----------------------------------------------------------------------------------
int main(void) {

	int key;
	int was_in_room;
	
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

	r.render_status_base(screen);
	r.render_text_base(screen, false);
	
	// Place the player on a random set of up stairs
	vector<int> stairLoc = m.getRandomStair(STAIRS_UP);

	// Create a new player at the stair location
	g_player = Player(stairLoc[0], stairLoc[1]);
	
	// Hack to force lighting in the initial room the player is in
	int initial_room = m.getRoomIdAt(g_player.x_pos, g_player.y_pos);
	if (initial_room != -1) {
		was_in_room = initial_room;
		m.changeRoomLitStatus(initial_room, true);		
	}
	
	// Loop until done.  Right now, 'done' = pressing Esc
	bool done = false;
	// Does the display need to be refreshed?
	bool update_display = true;
	
	// Main game loop
	while (done == false) {
		// Things to do when the display needs updating
		if (update_display == true) {
			// Light the space around the player
			m.changeLitStatusAround(g_player.x_pos, g_player.y_pos, true);
			
			// Check what room the player is in, if any
			int room_to_light = m.getRoomIdAt(g_player.x_pos, g_player.y_pos);
			
			// If the player was in a room but no longer is, then darken the room
			if(was_in_room != -1 && room_to_light == -1) {
				m.changeRoomLitStatus(was_in_room, false);
			}
			// If the player wasn't in a room but now is, then light up the room
			if(was_in_room == -1 && room_to_light != -1) {
				m.changeRoomLitStatus(room_to_light, true);
			}
			
			// Draw the world display area
			r.render_world_at_player(g_back_buffer, m, g_player.x_pos, g_player.y_pos);
			blit(g_back_buffer, screen, 0, 0, 0, 0, 240, 208);
			
			// Update is finished, make sure it's not done again until necessary
			update_display = false;
		}
		
		key = (readkey() >> 8);
		if (key == KEY_ESC) {
			done = true;
		}

		if (key == KEY_LEFT || key == KEY_RIGHT || key == KEY_UP || key == KEY_DOWN) 
		{
			// If the player is currently in a room, keep track of that room ID so we can darken
			// the room if the player leaves it on move
			was_in_room = m.getRoomIdAt(g_player.x_pos, g_player.y_pos);
			// Darken the current space around the player if not in a room
			if (was_in_room == -1) {
				m.changeLitStatusAround(g_player.x_pos, g_player.y_pos, false);
			}
		}
		if (key == KEY_LEFT) {
			if (m.isCarved(g_player.x_pos-1, g_player.y_pos) == true) {
				g_player.x_pos = g_player.x_pos -1;
				update_display = true;
			}
		}
		if (key == KEY_RIGHT) {
			if (m.isCarved(g_player.x_pos+1, g_player.y_pos) == true) {			
				g_player.x_pos = g_player.x_pos + 1;
				update_display = true;
			}
		}
		if (key == KEY_UP) {
			if (m.isCarved(g_player.x_pos, g_player.y_pos-1) == true) {
				g_player.y_pos = g_player.y_pos - 1;
				update_display = true;
			}
		}
		if (key == KEY_DOWN) {
			if (m.isCarved(g_player.x_pos, g_player.y_pos+1) == true)
			{
				g_player.y_pos = g_player.y_pos + 1;
				update_display = true;
			}
		}
	}
		
	unload_resources();
	set_gfx_mode(GFX_TEXT, 80, 25, 0, 0);

	return 0;
}
