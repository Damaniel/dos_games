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
#include "dungeon.h"
#include <stdio.h>
#include <stdlib.h>
#include <alloc.h>

//===========================================================================
// A basic dungeon generator using BSPs
//
// Right now, the results look rather...symmetrical, since I alternate
// between horizontal and vertical splits.  A better way to do it would
// be to pick a random split direction and position, and if the split
// results in two regions with unappealing aspect ratios, try splitting
// again.
//===========================================================================

extern DungeonSquare *g_dungeon;

// Deallocate all memory for the entire tree
void destroy_tree(TreeNode *node) {
	if (node->left != NULL) {
		destroy_tree(node->left);
		node->left = NULL;
	}
	if (node->right != NULL) {
		destroy_tree(node->right);
		node->right = NULL;
	}

	free(node->p);
	node->p = NULL;
	free(node->r);
	node->r = NULL;
	free(node);
	node = NULL;
}

//---------------------------------------------------------------------------
// perform_random_split
//
// Takes a partition (p) and splits it into two partitions (p1 and p2).
// p1 and p2 combined cover the same area as p, but represent either a
// horizontal or vertical split of that space.
//
// To ensure relatively even splitting, the type of split (horizontal or
// vertical, will alternate by depth, splitting within a relatively narrow
// range of center.
//---------------------------------------------------------------------------
void perform_random_split(Partition *p, Partition *p1, Partition *p2, int depth)
{
	int sx, sy;
	float minSplit = 0.35;
	float splitSize = 0.30;

	if (depth % 2 == 0) {		// Vertical split
		int min = (int)(minSplit * p->w);
		int size = (int)(splitSize * p->w);
		sx = rand() % size + min;
		p1->x = p->x;
		p1->y = p->y;
		p1->w = sx;
		p1->h = p->h;
		p2->x = p->x + sx;
		p2->y = p->y;
		p2->w = p->w - sx;
		p2->h = p->h;
	}
	else {						// Vertical split
		int min = (int)(minSplit * p->h);
		int size = (int)(splitSize * p->h);
		sy = rand() % size + min;
		p1->x = p->x;
		p1->y = p->y;
		p1->w = p->w;
		p1->h = sy;
		p2->x = p->x;
		p2->y = p->y + sy;
		p2->w = p->w;
		p2->h = p->h - sy;
	}
}

// Relevant variables
//
// p - The Partition associated with a node.  Contains the position
//     and size of the partition
// p1, p2 - the two partitions obtained when splitting p randomly
//          into two parts
// iter - specifies the current depth.  Each depth reduces the value gy 1.
//        When iter is 0, the tree won't be generated any deeper.
// n - the new tree node that will contain the passed in partition p.
//
TreeNode * create_split_node(Partition *p, int iter) {
	// Create a new node
	TreeNode *n = (TreeNode *)malloc(sizeof(TreeNode));
	// Set the partition of that node to p
	n->p = p;
	n->r = (Room*)malloc(sizeof(Room));
	// Set the leaves to NULL.  If we descend deeper, they'll point to
	// another node.
	n->left = NULL;
	n->right = NULL;
	// Set up two new partitions
	Partition *p1 = NULL;
	Partition *p2 = NULL;

	if (iter !=0 ) {
		// Create a pair of new Partitions based on splitting the
		// existing partition p into two new ones
		p1 = (Partition *)malloc(sizeof(Partition));
		p2 = (Partition *)malloc(sizeof(Partition));

		// *** splitting goes here - for now, just initialize ***
		perform_random_split(p, p1, p2, iter);

		// Call the function recursively
		n->left = create_split_node(p1, iter-1);
		n->right = create_split_node(p2, iter-1);
	}
	return n;
}

// Create rooms based on the partitions at the bottom leaves of the tree

// A room is made by placing padding randomly around each of the 4 sides of
// the partition.  The amount of padding varies depending on the size of the
// room, and a minimum / maximum room size is specified.
//
// First, the partition needs to be checked to see if it's even big enough
// for the smallest size room.  If not, don't create one.
//
// Second, a candidate room size is created.  If either dimension of the
// room is too large for the partition, that dimension will be shrunk to
// the size of the room minus the minimum padding, and the top and left
// padding will be set to that minimum padding value.
//
// Third, given a room size:
//   - For each room dimension, subtract it from the partition dimension
//   - Then subtract the total minimum padding
//   - The remainder is the amount of extra padding that's needed.
//   - Pick a random number between 0 and that padding, and set the top or
//     left dimension to it + the minimum padding
void create_rooms(TreeNode *n) {

	if (n->left == NULL && n->right == NULL) {
		int requiredW = MIN_ROOM_SIZE + 2 * MIN_PADDING;
		int requiredH = MIN_ROOM_SIZE + 2 * MIN_PADDING;
		if (n->p->w < requiredW || n->p->h < requiredH) {
			n->r->x = INVALID_ROOM;
			n->r->y = INVALID_ROOM;
			n->r->w = INVALID_ROOM;
			n->r->h = INVALID_ROOM;
			return;
		}
		// Create a candidate room
		int xSize = (rand() % (MAX_ROOM_SIZE - MIN_ROOM_SIZE)) + MIN_ROOM_SIZE;
		int ySize = (rand() % (MAX_ROOM_SIZE - MIN_ROOM_SIZE)) + MIN_ROOM_SIZE;
		int leftPadding = MIN_PADDING;
		int topPadding = MIN_PADDING;

		if (xSize > n->p->w - (2 * MIN_PADDING)) {
			xSize = n->p->w - (2 * MIN_PADDING);
			leftPadding = MIN_PADDING;
		}
		else {
			int excessPadding = n->p->w - xSize - (2 * MIN_PADDING);
			if (excessPadding > 0 ) {
				do {
					leftPadding = MIN_PADDING + rand() % excessPadding;
				} while (leftPadding > (n->p->w / 2) || (xSize + leftPadding) <= (n->p->w /2));

			}
			else {
				leftPadding = MIN_PADDING;
			}
		}

		if (ySize > n->p->h - (2 * MIN_PADDING)) {
			ySize = n->p->h - (2 * MIN_PADDING);
			topPadding = MIN_PADDING;
		}
		else {
			int excessPadding = n->p->h - ySize - (2 * MIN_PADDING);
			if (excessPadding > 0 ) {
				do {
					topPadding = MIN_PADDING + rand() % excessPadding;
				} while (topPadding > (n->p->h / 2) || (ySize + topPadding) <= (n->p->h / 2));
			}
			else {
				topPadding = MIN_PADDING;
			}
		}

		n->r->x = n->p->x + leftPadding;
		n->r->y = n->p->y + topPadding;
		n->r->w = xSize;
		n->r->h = ySize;
	}
	else {
		create_rooms(n->left);
		create_rooms(n->right);
	}
}

void connect_rooms(TreeNode *n, DungeonSquare *dungeon) {
	// Quick and dirty since our trees aren't two way -
	// just link the centers of the children together.  The
	// rooms are guaranteed to contain the center of the
	// region, so this will always link rooms.
	int seg1x, seg1y;
	int seg2x, seg2y;

	if (n->left != NULL && n->right != NULL) {
		seg1x = n->left->p->x + (n->left->p->w / 2);
		seg1y = n->left->p->y + (n->left->p->h / 2);
		seg2x = n->right->p->x + (n->right->p->w / 2);
		seg2y = n->right->p->y + (n->right->p->h / 2);
		link_segments(dungeon, seg1x, seg1y, seg2x, seg2y);

		connect_rooms(n->left, dungeon);
		connect_rooms(n->right, dungeon);
	}
}

void initialize_dungeon(DungeonSquare *dungeon) {
	int offset;
	for(int i=0; i < DUNGEON_WIDTH; i++) {
		for(int j = 0; j < DUNGEON_HEIGHT; j++) {
			offset = j * DUNGEON_WIDTH + i;
			dungeon[offset].carved = UNCARVED;
			dungeon[offset].upStair = 0;
			dungeon[offset].downStair = 0;
			dungeon[offset].reserved = 0;
			dungeon[offset].tileId = 0;
		}
	}
}

void carve_at(DungeonSquare *dungeon, int x, int y) {
	if (x >=0 && x < DUNGEON_WIDTH && y >=0 && y < DUNGEON_HEIGHT) {
		dungeon[y * DUNGEON_WIDTH + x].carved = CARVED;
	}
}

void populate_dungeon(TreeNode *n, DungeonSquare *dungeon) {
	if (n->left == NULL && n->right == NULL) {
		for (int i = n->r->x; i < n->r->x + n->r->w; i++) {
			for (int j = n->r->y; j < n->r->y + n->r->h; j++) {
				carve_at(dungeon, i, j);
			}
		}
	}
	else {
		populate_dungeon(n->left, dungeon);
		populate_dungeon(n->right, dungeon);
	}
}

void print_dungeon(DungeonSquare *dungeon) {
	for (int i = 0; i < DUNGEON_HEIGHT; i++) {
		for (int j = 0; j < DUNGEON_WIDTH; j++) {
			if (dungeon[i*DUNGEON_WIDTH + j].carved == UNCARVED) {
				printf("X");
			}
			else {
				printf(" ");
			}
		}
		printf("\n");
	}
}

//--------------------------------------------------------------------------------------------
// link_segments
//
// Carves a line from (seg1x, seg1y) to (seg2x, seg2y).
//
// Algorithm:
//   Draw a line from (seg1x, seg1y) to (seg2x, seg1y)
//   Draw a line from (seg2x, seg1y) to (seg2x, seg2y)
//--------------------------------------------------------------------------------------------
void link_segments(DungeonSquare *dungeon, int seg1x, int seg1y, int seg2x, int seg2y) {
	for (int i = seg1x ; i < seg2x; i++) {
		carve_at(dungeon, i, seg1y);
	}
	for (int j = seg1y; j < seg2y ; j++) {
		carve_at(dungeon, seg2x, j);
	}
}
