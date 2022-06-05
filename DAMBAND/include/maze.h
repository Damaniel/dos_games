#ifndef __MAZE_H__
#define __MAZE_H__

#define MAZE_WIDTH      80
#define MAZE_HEIGHT     50
#define PARTITION_SIZE  10
#define X_PARTITIONS    (MAZE_WIDTH / PARTITION_SIZE)
#define Y_PARTITIONS    (MAZE_HEIGHT / PARTITION_SIZE)
#define TOTAL_PARTITIONS     ((MAZE_WIDTH / PARTITION_SIZE ) * (MAZE_HEIGHT / PARTITION_SIZE))
#define MAX_ROOMS       (TOTAL_PARTITIONS / 3)
#define MIN_ROOM_SIZE   5
#define MAX_ROOM_SIZE   8

typedef struct {
    char carved;
    char is_lit;
} MazeSquare;

typedef struct {
    char partition_x;
    char partition_y;
    char x_size;
    char y_size;
    char x_pos;
    char y_pos;
    char is_lit;
} Room;

extern Room g_rooms[MAX_ROOMS];
extern char g_num_rooms;

extern MazeSquare g_maze[MAZE_WIDTH][MAZE_HEIGHT];

extern void light_room(char index);
extern void print_maze(void);
extern void generate_maze(void);

#endif