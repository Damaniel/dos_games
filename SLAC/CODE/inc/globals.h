#ifndef __GLOBALS_H__
#define __GLOBALS_H__

/* Common includes */
#include <stdlib.h>
#include <time.h>
#include <allegro.h>

/* Header for the datafile */
#include "dat.h"

/* The datafile itself */
extern DATAFILE *g_game_data;

/* The maze is made up of carved out parts of an array of blocks, so each space
 * in the array is either carved or solid (that is, uncarved). */ 
#define BLOCK_SOLID  0
#define BLOCK_CARVED 1

#endif
