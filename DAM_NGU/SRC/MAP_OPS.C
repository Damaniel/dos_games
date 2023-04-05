#include "../INCLUDE/DAM_NGU.H"

MapSquare g_map_page[MAP_WIDTH][MAP_HEIGHT];
MapLocation g_map_location;

// Dumps map data directly into the map page structure
int load_map_from_file(char *name) {
    FILE *fp;
    int i, j;
    char header[8];

    fp = fopen(name, "rb");
    if (fp == NULL) {
        printf("Invalid file name!\n");
        return -1;
    }

    fread(header, 1, 8, fp);
    if (header[0] != 'D' || header[1] != 'M' || header[2] != 'A' || header[3] != 'P') {
        printf("Invalid header!\n");
        return -1;
    }

    if(header[4] != MAP_WIDTH || header[5] != MAP_HEIGHT) {
        printf("Invalid size!\n");
        return -1;
    }

    for(j=0;j<MAP_HEIGHT;j++) {
        for(i=0;i<MAP_WIDTH;i++) {
            g_map_page[i][j].bg_tile_layer_idx = fgetc(fp);
            g_map_page[i][j].trans_tile_layer_idx = fgetc(fp);
            g_map_page[i][j].flags = fgetc(fp);
        }
    }

    fclose(fp);
    return 0;
}

int load_map_from_coordinates(int x, int y) {
    char fn[18];
    
    if (x>9999 || y >9999) {
        printf("Map coordinates way too big!\n");
        return -1;
    }

    sprintf(fn, "maps\\%04d%04d.map", x, y);
    // Load the map with the specified name
    return load_map_from_file(fn);
}

void init_map(void) {
    int i, j;
    for (i=0; i<MAP_WIDTH;i++) {
        for (j=0; j<MAP_HEIGHT; j++) {
            g_map_page[i][j].bg_tile_layer_idx = 0;
            g_map_page[i][j].trans_tile_layer_idx = 0;
        }
    }
}
