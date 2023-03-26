#include "player.h"

Player g_player;

void init_player(void) {
    g_player.name[0] = '\0';
    g_player.level = 1;
    g_player.x_pos = 8;
    g_player.y_pos = 8;
    g_player.light_radius = STANDARD_RADIUS;
}