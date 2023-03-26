#ifndef __MAZE_H__
#define __MAZE_H__

#define MIN_SIZE_PCT    45

typedef enum {
    MAZE_FLOOR,
    MAZE_WALL,
    MAZE_UP_STAIRS,
    MAZE_DOWN_STAIRS,
    NUM_MAZE_SQUARE_TYPES
} SquareType;

typedef struct {
    unsigned char squareType;
    unsigned char floodFilled;
} MazeSquare;

extern MazeSquare *g_maze_data;
extern MazeSquare *g_temp_maze_data;
extern MazeSquare *g_fill_data;
extern int g_maze_width;
extern int g_maze_height;

void iterate_maze(int r1_cutoff, int r2_cutoff);
void init_random_maze(int pct_walls);
void flood_fill_at(int x, int y);
void fill_random_region(void);
int check_fill_percentage(void);
void backfill_maze(void);
void clear_maze(MazeSquare *m);
void generate_maze(void);
void print_maze(void);
void set_maze_square(MazeSquare *m, int x, int y, char square_type);
MazeSquare *get_maze_square(MazeSquare *m, int x, int y);
void set_filled(MazeSquare *m, int x, int y, char filled);
int init_maze_structs(int width, int height);
void free_temp_maze_structs(void);
void free_maze_structs(void);

#endif