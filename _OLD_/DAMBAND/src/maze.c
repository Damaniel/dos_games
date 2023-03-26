/*
    - Repeat
        - Link room 0 to room 1
        - link room 1 to room 2
        - And so on, until room n-1 to room n
        - And then (num_rooms / 4) additional random pairs, not already directly linked (i.e room a and room b aren't adjacent in the list)
        - For each of these
          - Pick a point in each of the two rooms, and create 2 lines that
            connect the two rooms together
*/

#include <stdio.h>
#include <stdlib.h>
#include "maze.h"

/* Globally allocated memory */
// Maze - 8000 bytes
// Rooms - ~100 bytes
// Other - 1 byte

Room g_rooms[MAX_ROOMS];
char g_num_rooms;

MazeSquare g_maze[MAZE_WIDTH][MAZE_HEIGHT];

int add_rooms(void) {
    
    unsigned char num_rooms = 0;
    unsigned char i, j;
    unsigned char add_room;
    unsigned char candidate_x, candidate_y;

    for (i=0; i < MAX_ROOMS * 2; i++) {
        add_room = 1;
        // if we've already got enough rooms, stop.
        if (num_rooms > MAX_ROOMS) {
                add_room = 0;
        }
        else {
            // Pick a position
            candidate_x = rand() % X_PARTITIONS;
            candidate_y = rand() % Y_PARTITIONS;
            // See if it's already taken
            for (j=0; j < num_rooms; j++ ) {            
                if(g_rooms[j].partition_x == candidate_x && g_rooms[j].partition_y == candidate_y) {
                    add_room = 0;
                }
            }
            // if not, add it
            if (add_room) {
                g_rooms[num_rooms].partition_x = candidate_x;
                g_rooms[num_rooms].partition_y = candidate_y;
                g_rooms[num_rooms].is_lit = 1;
                num_rooms++;
            }
        }
    }
     
    // For each room in the room list
    //  - Pick a width and height
    //  - Pick an x and y offset
    for (i=0; i < num_rooms; i++ ) {
        g_rooms[i].x_size = (rand() % (MAX_ROOM_SIZE - MIN_ROOM_SIZE + 1)) + MIN_ROOM_SIZE;
        g_rooms[i].y_size = (rand() % (MAX_ROOM_SIZE - MIN_ROOM_SIZE + 1)) + MIN_ROOM_SIZE;
        g_rooms[i].x_pos = (rand() % (PARTITION_SIZE - g_rooms[i].x_size - 1))  + 1;
        g_rooms[i].y_pos = (rand() % (PARTITION_SIZE - g_rooms[i].y_size - 1)) + 1;
    }

    //printf("total number of rooms = %d\n", num_rooms);
    //for(i=0; i < num_rooms; i++) {
        //printf("Room %d:\n - in partition (%d, %d)\n", i+1, g_rooms[i].partition_x, g_rooms[i].partition_y);
        //printf(" - size (%d x %d)\n - position (%d, %d)\n", g_rooms[i].x_size, g_rooms[i].y_size, g_rooms[i].x_pos, g_rooms[i].y_pos);
    //}   

    return num_rooms;
}

void carve_h(unsigned char x1, unsigned char x2, unsigned char y) {
    unsigned char i, start_x, end_x;

    if (x1 < x2) {
        start_x = x1;
        end_x = x2;
    }
    else {
        start_x = x2;
        end_x = x1;
    }
    
    for (i=start_x; i <= end_x; i++) {
        g_maze[i][y].carved = 1;
    }
}

void carve_v(unsigned char x, unsigned char y1, unsigned char y2) {
    unsigned char i, start_y, end_y;

    if (y1 < y2) {
        start_y = y1;
        end_y = y2;
    }
    else {
        start_y = y2;
        end_y = y1;
    }

    for(i=start_y; i <= end_y; i++) {
        g_maze[x][i].carved = 1;
    }
}

void carve_passages(void) {
    unsigned char i;
    unsigned char r1_x, r1_y, r2_x, r2_y;
    unsigned char intersect_x, intersect_y;

    for (i=0; i < g_num_rooms - 1; i++) {
        // Connect room i and room i+1 together

        // Pick a random spot in room i
        r1_x = (rand() % g_rooms[i].x_size) + g_rooms[i].x_pos + (g_rooms[i].partition_x * PARTITION_SIZE);
        r1_y = (rand() % g_rooms[i].y_size) + g_rooms[i].y_pos + (g_rooms[i].partition_y * PARTITION_SIZE);

        // Pick a random spot in room i+1
        r2_x = (rand() % g_rooms[i+1].x_size) + g_rooms[i+1].x_pos + (g_rooms[i+1].partition_x * PARTITION_SIZE);
        r2_y = (rand() % g_rooms[i+1].y_size) + g_rooms[i+1].y_pos + (g_rooms[i+1].partition_y * PARTITION_SIZE);

        if (rand() % 2 == 0) {
            // Use R1's Y value and R2's X value
            intersect_x = r2_x;
            intersect_y = r1_y;
            //printf("Linking (%d, %d) to (%d, %d) via (%d, %d)\n", r1_x, r1_y, r2_x, r2_y, intersect_x, intersect_y);
            carve_h(r1_x, intersect_x, intersect_y);
            carve_v(intersect_x, intersect_y, r2_y);
        }
        else {
            // Use R2's Y value and R1's X value
            intersect_x = r1_x;
            intersect_y = r2_y;
            //printf("Linking (%d, %d) to (%d, %d) via (%d, %d)\n", r1_x, r1_y, r2_x, r2_y, intersect_x, intersect_y);            
            carve_h(intersect_x, r2_x, intersect_y);
            carve_v(intersect_x, r1_y, intersect_y);
        }
    }
}

void carve_rooms(void) {
    unsigned char i, j, k;
    unsigned char start_x, start_y;

    for (i=0; i < g_num_rooms; i++ ) {
        start_x = g_rooms[i].partition_x * PARTITION_SIZE + g_rooms[i].x_pos;
        start_y = g_rooms[i].partition_y * PARTITION_SIZE + g_rooms[i].y_pos;
        // Carve the rooms
        for (j=start_x; j < start_x + g_rooms[i].x_size; j++) {
            for (k=start_y; k < start_y + g_rooms[i].y_size; k++) {
                g_maze[j][k].carved = 1;                
            }
        }
    }
}

char is_in_room(char x, char y) {
    char i;
    char r_t, r_l, r_b, r_r;

    for (i=0; i< g_num_rooms; i++) {
        r_l = g_rooms[i].partition_x * PARTITION_SIZE + g_rooms[i].x_pos;
        r_t = g_rooms[i].partition_y * PARTITION_SIZE + g_rooms[i].y_pos;
        r_r = r_l + g_rooms[i].x_size - 1;
        r_b = r_t + g_rooms[i].y_size - 1;
        if(x >= r_l && x <= r_r && y >= r_t && y <= r_b) {
            return i;
        }
    }

    return -1;
}

void light_room(char index) {
    char i, j;
    char r_x, r_y;

    r_x = g_rooms[index].partition_x * PARTITION_SIZE + g_rooms[index].x_pos;
    r_y = g_rooms[index].partition_y * PARTITION_SIZE + g_rooms[index].y_pos;
    for(i = r_x - 1; i < r_x + g_rooms[index].x_size + 1; i++ ) {
        for (j = r_y - 1; j < r_y + g_rooms[index].y_size + 1; j++ ) {
            g_maze[i][j].is_lit = 1;
        }
    }
}

void clear_maze(void) {
    unsigned char i, j;
    
    for (i=0; i < MAZE_WIDTH; i++ ) {
        for (j=0; j < MAZE_HEIGHT; j++) {
            g_maze[i][j].carved = 0;
            g_maze[i][j].is_lit = 1;
        }
    }
}

void remove_isolates(void) {
    // scan from left to right, top to bottom
    // if any uncarved square is surrounded by 3 or 4 carved squares, carve it.
    unsigned char i, j, num_carved;

    for (j = 1; j < MAZE_HEIGHT - 1; j++) {
        for(i = 1; i < MAZE_WIDTH - 1; i++) {
            if (g_maze[i][j].carved == 0) {
                num_carved = 0;
                if(g_maze[i][j-1].carved == 1)
                    num_carved++;
                if(g_maze[i][j+1].carved == 1)
                    num_carved++;
                if(g_maze[i-1][j].carved == 1)
                    num_carved++;
                if(g_maze[i+1][j].carved == 1)
                    num_carved++;
                
                if(num_carved >= 3) {
                    g_maze[i][j].carved = 1;
                }
            }
        }
    }   
}

void generate_maze(void) {
    clear_maze();
    g_num_rooms = add_rooms();
    carve_rooms();
    carve_passages();
    remove_isolates();
}

void print_maze(void) {
    unsigned char i, j;

    for (j=0; j < MAZE_HEIGHT; j++ ) {
        for (i=0; i < MAZE_WIDTH; i++ ) {
            if (g_maze[i][j].carved) {
                putchar(' ');
            }
            else {
                putchar('#');
            }
        }
        putchar('\n');
    }
}