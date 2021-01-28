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

#define MIN_PADDING	     2
#define MIN_ROOM_SIZE    3
#define MAX_ROOM_SIZE   12

#define DUNGEON_WIDTH   64
#define DUNGEON_HEIGHT  64

#define INVALID_ROOM    -1

#define UNCARVED	     0
#define CARVED	 	     1

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

typedef struct node {
	Partition *p;
	Room      *r;
	struct node *left;
	struct node *right;
} TreeNode;

// A bitfield that contains basic structural info about the dungeon
typedef struct {
	unsigned carved    : 1;
	unsigned upStair   : 1;
	unsigned downStair : 1;
	unsigned reserved  : 2;
	unsigned tileId    : 3;
} DungeonSquare;

// Prototypes
void link_segments(DungeonSquare *dungeon, int seg1x, int seg1y, int seg2x, int seg2y);
void destroy_tree(TreeNode *node);
void perform_random_split(Partition *p, Partition *p1, Partition *p2, int depth);
TreeNode * create_split_node(Partition *p, int iter);
void create_rooms(TreeNode *n);
void connect_rooms(TreeNode *n, DungeonSquare *dungeon);
void initialize_dungeon(DungeonSquare *dungeon);
void carve_at(DungeonSquare *dungeon, int x, int y);
void populate_dungeon(TreeNode *n, DungeonSquare *dungeon);
void print_dungeon(DungeonSquare *dungeon);

#endif