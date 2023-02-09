#include <allegro.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define FRAME_RATE 10
#define CENTER_TILE_X       100
#define CENTER_TILE_Y        76
#define TILE_WIDTH           48
#define TILE_HEIGHT          24
#define TILE_RADIUS           2

// By default, the 3 dimensions of the array represent (z, x, y)
// Rotation may change this. 

unsigned char map[5][7][7] = {
    {
        {1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1}

    },
    {
        {1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1},
        {1, 0, 0, 0, 0, 0, 0},
        {1, 0, 0, 0, 0, 0, 0},
        {1, 0, 0, 0, 0, 0, 0},
        {1, 0, 0, 0, 0, 0, 0}
    },
    {
        {1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1},
        {1, 0, 1, 0, 1, 0, 0},
        {1, 0, 0, 0, 0, 0, 0},
        {1, 0, 0, 0, 0, 0, 0},
        {1, 0, 0, 0, 0, 0, 0},
        {1, 0, 0, 0, 0, 0, 0}

    },
    {
        {1, 1, 1, 1, 1, 1, 1},
        {1, 0, 1, 1, 1, 0, 0},
        {1, 0, 0, 0, 0, 0, 0},
        {1, 0, 0, 0, 0, 0, 0},
        {1, 0, 0, 0, 0, 0, 0},
        {1, 0, 0, 0, 0, 0, 0},
        {1, 0, 0, 0, 0, 0, 0}
    },
    {
        {1, 1, 1, 1, 1, 1, 1},
        {1, 0, 0, 0, 0, 0, 0},
        {1, 0, 0, 0, 0, 0, 0},
        {1, 0, 0, 0, 0, 0, 0},
        {1, 0, 0, 0, 0, 0, 0},
        {1, 0, 0, 0, 0, 0, 0},
        {1, 0, 0, 0, 0, 0, 0}
    }
};

PALETTE g_game_pal;

BITMAP *g_buffer, *g_tile[3];
volatile unsigned long int g_frame_counter;
volatile int g_next_frame;
int g_position_changed;

// About the tile grid
// The 'center' tile (center of the 5x5x5 grid) is at 100, 76
// The 3 axes are as follows:
//
// x: line of positive slope, positive values go left/down
// y: line of negative slope, positive values go right/down
// x: goes straight up
//
//                  | |
//                  | z
//                  |
//                 /\
//              x /  \  y
//             / /    \  \
//              /      \
//
// Draw a region of the map, centered at tile (x, y, z)
void render_map_region(int cx, int cy, int cz) {
    int x, y, z;
    BITMAP *tile;
    int distance;

    for(z=-TILE_RADIUS; z <= TILE_RADIUS; z++) {
        for(x= -TILE_RADIUS; x <= TILE_RADIUS; x++){
            for(y= -TILE_RADIUS; y <= TILE_RADIUS;  y++) {
                if (abs(x) == 2 || abs(y) == 2 || abs(z) == 2) {
                    tile = g_tile[0];
                }
                else if (abs(x) == 1 || abs(y) == 1 || abs(z) == 1) {
                    tile = g_tile[1];
                }
                else {
                    tile = g_tile[2];
                }
                if(map[cz+z][cx+x][cy+y] != 0) {
                    draw_sprite(g_buffer, tile, CENTER_TILE_X + ((y-x) * (TILE_WIDTH / 2)), CENTER_TILE_Y + ((y+x) * (TILE_HEIGHT /2)) - (z * TILE_HEIGHT));
                }
            }
        }
    }
}

void int_handler(void) {
  /* do animation stuff here */
  g_frame_counter++;
  g_next_frame = 1;
}

END_OF_FUNCTION(int_handler);

int main(void) {
    int done = 0;
    int i = 0;
    int x, y, z;

    // Init the counters
    g_frame_counter = 0;
    g_next_frame = 1;

    allegro_init();
    install_keyboard();
    install_timer();

    install_int(int_handler, 1000/FRAME_RATE);
   
    set_gfx_mode(GFX_VGA, 320, 200, 0, 0);
    g_tile[0] = load_bitmap("block0.pcx", g_game_pal);
    g_tile[1] = load_bitmap("block1.pcx", g_game_pal);
    g_tile[2] = load_bitmap("block2.pcx", g_game_pal);
    g_buffer = create_bitmap(320, 200);
    g_position_changed = 1;

    set_palette(g_game_pal);

    clear_to_color(g_buffer, 1);
    clear_to_color(screen, 1);

    x = 2;
    y = 2;
    z = 2;

    while(!done) { 
        while(!g_next_frame) {
            rest(1);
        }

        if (g_position_changed) {
            clear_to_color(g_buffer, 1);
            render_map_region(x, y, z);
            g_position_changed = 0;
        }

        blit(g_buffer, screen, 0, 0, 0, 0, 320, 200);

        // check for input
        if (key[KEY_ESC])
            done = 1;

        if (key[KEY_A]) {
            x++;
            g_position_changed = 1;
        }
        // Reset the next frame counter
        g_next_frame = 0;
    }

    set_gfx_mode(GFX_TEXT, 80, 25, 0, 0);
    return 0;

}