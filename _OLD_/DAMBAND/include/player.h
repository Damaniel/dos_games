#ifndef __PLAYER_H__
#define __PLAYER_H__

#define NO_LIGHT           0
#define STANDARD_RADIUS    1
#define ENHANCED_RADIUS    2

typedef struct {
    char name[12];
    char level;
    char x_pos;
    char y_pos;
    char light_radius;
} Player;

extern Player g_player;

extern void init_player(void);
#endif