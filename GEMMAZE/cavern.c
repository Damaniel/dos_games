#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "cavern.h"

// The actual maze
MazeSquare *g_maze_data;

// Used for iterating the cellular automata and for doing flood fills
MazeSquare *g_temp_maze_data;

int g_maze_width;
int g_maze_height;

void iterate_maze(int r1_cutoff, int r2_cutoff) {
    int i, j, ii, jj;
    int neighbors_d1, neighbors_d2;
    MazeSquare *m;

    for (j=1; j<g_maze_height-1; j++) {
        for(i=1; i<g_maze_width-1; i++) {
            neighbors_d1 = 0;
            neighbors_d2 = 0;

            for(jj=j-1; jj<=j+1; jj++) {
                for(ii=i-1; ii<=i+1; ii++) {
                    m = get_maze_square(g_maze_data, ii, jj);
                    if (m->squareType == MAZE_WALL)
                        neighbors_d1++;
                }
            }

            for(jj=j-2; jj<=j+2; jj++) {
                for(ii=i-2; ii<=i+2; ii++) {
                    if(abs(ii-i)==2 && abs(jj-j)==2) {
                        continue;
                    }
                    if(ii < 0 || ii >= g_maze_width || jj < 0 || jj >= g_maze_height) {
                        continue;
                    }
                    m = get_maze_square(g_maze_data, ii, jj);
                    if(m->squareType == MAZE_WALL)
                        neighbors_d2++;
                }
            }

            if(r2_cutoff == -1) {
                if (neighbors_d1 >= r1_cutoff)
                    set_maze_square(g_temp_maze_data, i, j, MAZE_WALL);
                else
                    set_maze_square(g_temp_maze_data, i, j, MAZE_FLOOR);
            } else {
                if (neighbors_d1 >= r1_cutoff || neighbors_d2 <= r2_cutoff)
                    set_maze_square(g_temp_maze_data, i, j, MAZE_WALL);
                else
                    set_maze_square(g_temp_maze_data, i, j, MAZE_FLOOR);
            }
        }
    }

    for(j=1; j<g_maze_height-1; j++) {
        for(i=1; i<g_maze_width-1; i++) {
            m = get_maze_square(g_temp_maze_data, i, j);
            set_maze_square(g_maze_data, i, j, m->squareType); 
        }
    }
}

void set_maze_square(MazeSquare *m, int x, int y, char squareType) {
    m[y * g_maze_width + x].squareType = squareType;
}

void set_filled(MazeSquare *m, int x, int y, char filled) {
    m[y * g_maze_width + x].floodFilled = filled;
}

void init_random_maze(int pct_walls) {
    int i, j;

    for(i=0; i<g_maze_width;i++) {
        set_maze_square(g_maze_data, i, 0, MAZE_WALL);
        set_maze_square(g_maze_data, i, g_maze_height-1, MAZE_WALL);
    }

    for(i=0; i<g_maze_height;i++) {
        set_maze_square(g_maze_data, 0, i, MAZE_WALL);
        set_maze_square(g_maze_data, g_maze_width-1, i, MAZE_WALL);
    }

    for(j=1; j<g_maze_height-1; j++) {
        for(i=1; i<g_maze_width-1; i++) {
            if ((rand() % 100) <= pct_walls)
                set_maze_square(g_maze_data, i, j, MAZE_WALL);
            else 
                set_maze_square(g_maze_data, i, j, MAZE_FLOOR);
        }
    }
}

// Uses g_temp_maze_data to do a flood fill
void flood_fill_at(int x, int y) {
    MazeSquare *m;

    set_maze_square(g_temp_maze_data, x, y, MAZE_WALL);
    set_filled(g_temp_maze_data, x, y, 1);

    m = get_maze_square(g_temp_maze_data, x-1, y);
    if (m->squareType == MAZE_FLOOR)
        flood_fill_at(x-1, y);
    m = get_maze_square(g_temp_maze_data, x+1, y);
    if (m->squareType == MAZE_FLOOR)
        flood_fill_at(x+1, y);        
    m = get_maze_square(g_temp_maze_data, x, y-1);
    if (m->squareType == MAZE_FLOOR)
        flood_fill_at(x, y-1);        
    m = get_maze_square(g_temp_maze_data, x, y+1);
    if (m->squareType == MAZE_FLOOR)
        flood_fill_at(x, y+1);                
}

void fill_random_region(void) {
    char squareFound;
    int i, j, x, y;
    MazeSquare *m, *m2;

    squareFound = 0;

    while(squareFound == 0) {
        x = rand() % g_maze_width;
        y = rand() % g_maze_height;
        m = get_maze_square(g_temp_maze_data, x, y);
        if (m->squareType == MAZE_FLOOR)
            squareFound = 1;
    }

    for(j=0; j<g_maze_height; j++) {
        for (i=0; i<g_maze_width; i++) {
            m2 = get_maze_square(g_maze_data, i, j);
            set_maze_square(g_temp_maze_data, i, j, m2->squareType);
            set_filled(g_temp_maze_data, i, j, 0);
        }
    }

    flood_fill_at(x, y);
}

int check_fill_percentage(void) {
    int filledCount;
    int totalSize;
    int i, j, percentage;
    MazeSquare *m;

    filledCount = 0;
    totalSize = (g_maze_width - 2) * (g_maze_height - 2);

    for(j=1; j<g_maze_height-1; j++) {
        for(i=1; i<g_maze_width-1; i++) {
            m = get_maze_square(g_temp_maze_data, i, j);
            if (m->floodFilled == 1) {
                filledCount++;
            }
        }
    }

    percentage = (int)(((float)filledCount / (float)totalSize) * 100);
    return percentage;
}

void backfill_maze(void) {
    int i, j;
    MazeSquare *m;

    for(j=0; j<g_maze_height;j++) {
        for(i=0; i<g_maze_width; i++) {
            m = get_maze_square(g_temp_maze_data, i, j);
            if(m->floodFilled == 0)
                set_maze_square(g_maze_data, i, j, MAZE_WALL);
        }
    }
}

void clear_maze(void) {
    int i, j;

    for (j=0; j<g_maze_height; j++) {
        for (i=0; i<g_maze_width; i++) {
            set_maze_square(g_maze_data, i, j, MAZE_FLOOR);
            set_filled(g_maze_data, i, j, 0);
        }
    }
}

void generate_maze() {
    int i, pct;
    char validMaze;

    while (validMaze == 0) {
        clear_maze();
        init_random_maze(40);
        for(i=0; i<4; i++)     
            iterate_maze(5, 2);
        for(i=0; i<3; i++)
            iterate_maze(5, -1);
        fill_random_region();
        pct = check_fill_percentage();
        if(pct >= MIN_SIZE_PCT)
            validMaze = 1;
    }
    backfill_maze();
}

MazeSquare *get_maze_square(MazeSquare *m, int x, int y) {
    return &(m[y * g_maze_width + x]);
}

void print_maze(void) {
    int i, j;
    MazeSquare *square;

    for(j=0;j<g_maze_height;j++) {
        for (i=0;i<g_maze_width;i++) {
            square = get_maze_square(g_maze_data, i, j);
            if (square->squareType == MAZE_FLOOR)
                printf(" ");
            else if (square->squareType == MAZE_WALL)
                printf("#");
        }
        printf("\n");
    }
}

int init_maze_structs(int width, int height) {
    g_maze_width = width;
    g_maze_height = height;
    g_maze_data = malloc(g_maze_width*g_maze_height*sizeof(MazeSquare));
    if (g_maze_data == NULL) {
        return -1;
    }
    g_temp_maze_data = malloc(g_maze_width*g_maze_height*sizeof(MazeSquare));
    if(g_temp_maze_data == NULL) {
        // Free the sucessfully allocated maze data
        free(g_temp_maze_data);
        return -1;
    }
    return 0;

}

void free_maze_structs() {
    if(g_maze_data != NULL)
        free(g_maze_data);
    if(g_temp_maze_data != NULL)
        free(g_temp_maze_data);
}

int main(void) {

    srand(time(NULL));

    init_maze_structs(70, 20);
    generate_maze();
    print_maze();
    free_maze_structs();

    return 0;
}