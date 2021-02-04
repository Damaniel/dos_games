//==========================================================================================
//   Daminiband
//
//   Copyright (c) 2021 Shaun Brandt / Holy Meatgoat Software
//
//   Permission is hereby granted, free of charge, to any person obtaining a copy
//   of this software and associated documentation files (the "Software"), to deal
//   in the Software without restriction, including without limitation the rights
//   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//   copies of the Software, and to permit persons to whom the Software is
//   furnished to do so, subject to the following conditions:
//
//   The above copyright notice and this permission notice shall be included in all
//   copies or substantial portions of the Software.
//
//   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//   SOFTWARE.
//==========================================================================================
#ifndef __DUNGEON_H__
#define __DUNGEON_H__

#define LEFT	0
#define RIGHT	1

// The minimum border around a partition that an embedded room can't encroach
// in.
#define MIN_PADDING	     2

// Room size ranges.  
#define MIN_ROOM_SIZE    5
#define MAX_ROOM_SIZE   16

// The size of the dungeon.  
#define DUNGEON_WIDTH   64
#define DUNGEON_HEIGHT  64

// Used to specify that a room can't be created in a partition
#define INVALID_ROOM    -1

// The carved status of a dungeon square
#define UNCARVED	     0
#define CARVED	 	     1

// A basic 'position'/'dimension' structure.  It's used for both partitions
// and the rooms defined inside of them
typedef struct {
	int x;
	int y;
	int w;
	int h;
} Box;

// Partitions and Rooms use the same variable provided by Box,
// I'll just reuse it for both
typedef Box Partition;
typedef Box Room;

// A basic tree node.  A node contains a partition, which is a subset of the
// total area of the dungeon, and a room, which is a carved area which is a
// subset of the partition.
typedef struct node {
	Partition *p;
	Room      *r;
	struct node *left;
	struct node *right;
} TreeNode;

// A bitfield that contains basic structural info about the dungeon
typedef struct {
	unsigned carved    : 1;		// has this square been carved
	unsigned upStair   : 1;		// are there up stairs here?
	unsigned downStair : 1;		// are there down stairs here?
	unsigned reserved  : 2;		// Nothing yet
	unsigned tileId    : 3;		// An index into a tile array
} DungeonSquare;

// Since DungeonSquares are almost used in the context of a list of them,
// we'll call DungeonSquare * 'Dungeon' since it better represents what a 
// array of DungeonSquares are.
typedef DungeonSquare* Dungeon;

// Prototypes
void link_segments(Dungeon dungeon, int seg1x, int seg1y, int seg2x, int seg2y);
void destroy_tree(TreeNode *node);
void perform_random_split(Partition *p, Partition *p1, Partition *p2, int depth);
void create_rooms(TreeNode *n);
void connect_rooms(TreeNode *n, Dungeon dungeon);
void initialize_dungeon(Dungeon dungeon);
void carve_at(Dungeon dungeon, int x, int y);
void populate_dungeon(TreeNode *n, Dungeon dungeon);
void print_dungeon(Dungeon dungeon);
TreeNode * create_split_node(Partition *p, int iter);

#endif