#ifndef __MAP_OPS_H__
#define __MAP_OPS_H__

typedef struct {
    unsigned char bg_tile_layer_idx;
    unsigned char trans_tile_layer_idx;
    unsigned char flags;    // visibility, movement, etc
} MapSquare;

typedef struct {
    unsigned char map_page_x;
    unsigned char map_page_y;
    unsigned char p_x;
    unsigned char p_y;
} MapLocation;

int load_map_from_coordinates(int x, int y);
int load_map_from_file(char *name);
void init_map(void);

#endif