//==========================================================================================
//   Secret Legacy of the Ancient Caves (SLAC)
//
//   Copyright (c) 2020-2021 Shaun Brandt / Holy Meatgoat Software
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
#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <stack>
#include <iostream>
#include <algorithm>
#include <allegro.h>
#include "globals.h"
#include "maze.h"

//------------------------------------------------------------------------------
// Maze constructor
//------------------------------------------------------------------------------
Maze::Maze(int x, int y) {
	/* If the maze is of even size, add an additional row/column to ensure 
	 * a border surrounds the entire maze. */
	rows = y % 2 == 0 ? y + 1 : y;
	cols = x % 2 == 0 ? x + 1 : x;
	init();
}

//------------------------------------------------------------------------------
// addStairs
//
// Adds the specified number of up and down stairs to the maze.
//
// Notes:
//  - More than one set of stairs can't be in the same room
//  - If there are more combined up and down stairs than rooms
//  - Stairs can only be generated in rooms
//  - The location that stairs can be placed must be entirely surrounded
//    by either room spaces or walls.
//------------------------------------------------------------------------------
void Maze::addStairs(int numUpStairs, int numDownStairs) {
	int numRooms = roomId - STARTING_ROOM - 1;
	
	if (numUpStairs + numDownStairs > numRooms) {
		cout << "not enough rooms!" << endl;
		return;
	}
	
	vector<int> selectedRooms;
	vector<int>::iterator roomIt;
	
	// Iterate through up stairs
	for (int i=0; i < numUpStairs; i++) {
		bool valid;
		int candidateRoom;
		// Pick a random room and check to see if there's already stairs in it
		do {
			valid = true;
			candidateRoom = rand() % numRooms + STARTING_ROOM;
			for(vector<int>::iterator it = selectedRooms.begin(); it != selectedRooms.end(); it++) {
				if (*it == candidateRoom) {
					valid = false;
				}
			}
		}	
		while (valid == false);

		// When a candidate room is found, create the stairs there.
		Room r = getRoom(candidateRoom);	
		placeStairs(candidateRoom, STAIRS_UP);
		selectedRooms.push_back(candidateRoom);
	}
	
	// Repeat the same process for down stairs
	for (int i=0; i < numDownStairs; i++) {
		bool valid;
		int candidateRoom;
		do {
			valid = true;
			candidateRoom = rand() % numRooms + STARTING_ROOM;
			for(vector<int>::iterator it = selectedRooms.begin(); it != selectedRooms.end(); it++) {
				if (*it == candidateRoom) {
					valid = false;
				}
			}
		}	
		while (valid == false);
		Room r = getRoom(candidateRoom);	
		placeStairs(candidateRoom, STAIRS_DOWN);
		selectedRooms.push_back(candidateRoom);
	}	
}

//------------------------------------------------------------------------------
// carve
//
// Carves a square out of the maze.
//
// Notes:
//   A 'carved' square can either be a passageway, or part of a room.
//------------------------------------------------------------------------------
void Maze::carve(int x, int y, int tag) {
	m[y*cols + x].tag = tag;
	m[y*cols + x].carved = true;
}

//------------------------------------------------------------------------------
// carveDirection
//
// Carves the maze in a particular direction.
//
// Notes:
//   Due to the way that mazes are stored, carving in a direction requires 
//   carving three squares - the source square, the target square, and the 
//   square between them.
//------------------------------------------------------------------------------
void Maze::carveDirection(int x, int y, int direction, int tag) {
	if (direction == DIRECTION_NORTH) {
		carve(x, y, tag);
		carve(x, y-1, tag);
		carve(x, y-2, tag);
	}
	if (direction == DIRECTION_SOUTH) {
		carve(x, y, tag);
		carve(x, y+1, tag);
		carve(x, y+2, tag);
	}
	if (direction == DIRECTION_EAST) {
		carve(x, y, tag);
		carve(x+1, y, tag);
		carve(x+2, y, tag);
	}
	if (direction == DIRECTION_WEST) {
		carve(x, y, tag);
		carve(x-1, y, tag);
		carve(x-2, y, tag);
	}	
}

//------------------------------------------------------------------------------
// changeLitStatus
//
// Marks a maze square as lit or unlit.  Also marks the square as visited,
// to enable fog of war.
//------------------------------------------------------------------------------
void Maze::changeLitStatusAt(int x, int y, bool lit) {
	m[y * cols + x].isLit = lit;
	m[y * cols + x].wasSeen = true;
}

//------------------------------------------------------------------------------
// createRoom
// 
// Attempts to create a room in the maze, according to a specific set of
// criteria.
//
// Notes:
//	 To fit in the maze, the room must:
//		- be odd in height and width
//		- be offset by an odd amount
//		- fully fit within the bounds of the maze, leaving at least one 
//        uncarved square on each side of the maze area
//      - not intersect with any existing room
//	 If any of these aren't true, the room won't be created.
//------------------------------------------------------------------------------
bool Maze::createRoom(int x, int y, int w, int h) {

	// The room must be odd in width and height
	if (w % 2 == 0 || h % 2 == 0) {
		return false;
	}

	// The room must be offset by an odd amount
	if (x % 2 == 0 || y % 2 == 0) {
		return false;
	}
	
	// The room can't be pressed against any edge of the maze (it makes 
	// generation of passages more consistent)
	if (x <3 || y < 3 || (x + w) > (cols - 3) || (y + h) > (rows - 3)) {
		return false;
	}
	
	// Check for overlap with existing rooms.  If there is any, don't 
	// make the room.
	for (int i = x; i < x+w; i++ ) {
		for (int j = y; j < y+h; j++ ) {
			if (m[j*cols + i].tag >= STARTING_ROOM) {
				return false;
			}
		}
	}
		
	// Since all the previous critera were met, go ahead and carve the new room.
	for (int i = x; i < x+w; i++ ){
		for (int j = y; j < y+h; j++ ){		
			carve(i, j, roomId);
		}
	}
	
	// Save the room information for later edge detection and wall removal
	rooms.push_back(Room(roomId, x, y, w, h));
	roomId++;	

	return true;
			
}

//------------------------------------------------------------------------------
// generatePassages
//
// Creates passageways in any uncarved non-room space using a standard maze
// generation algorithm.
//
// Notes:
//   Uses a recursive backtracker to generate a maze.  It starts by carving a 
//   maze as greedily as possible, until there are no adjacent squares to carve 
//   to.  Then, the maze is systematically scanned over, and any uncarved spaces
//   that are adjacent to carved ones will be attached to one of the adjacent 
//   carved spaces.  The result is that every space in the maze should be 
//   connected. 
//------------------------------------------------------------------------------
void Maze::generatePassages(int x, int y) {
	int curX = x;
	int curY = y;
	int direction;
	bool done = false;
	vector<int> directions;
	stack< pair<int, int> > visited;
	
	while (!done) {
		pair<int, int> cur = make_pair(curX, curY);
		
		// Find uncarved adjacent spaces to the current one
		getDirections(directions, curX, curY);
		// If there aren't any, look to see if we have any others to look at
		// on the stack
		if (directions.size() == 0) {
			// If not, everything has been carved
			if (visited.size() == 0) {
				done = true;
			}
			else {
				// Otherwise, pull one off the top of the stack and start 
				// carving from there
				pair<int, int> top = visited.top();
				visited.pop();
				curX = top.first;
				curY = top.second;
			}
		}
		else {
			// If we found at least one adjacent uncarved space, pick one
			// and carve in that direction.
			visited.push(cur);
			direction = directions[rand() % directions.size()];
			if (direction == DIRECTION_NORTH) {
				carveDirection(curX, curY, DIRECTION_NORTH, PASSAGE);
				curY = curY - 2;
			}
			else if (direction == DIRECTION_SOUTH) {
				carveDirection(curX, curY, DIRECTION_SOUTH, PASSAGE);
				curY = curY + 2;
			}
			else if (direction == DIRECTION_EAST) {
				carveDirection(curX, curY, DIRECTION_EAST, PASSAGE);
				curX = curX + 2;
			}
			else if (direction == DIRECTION_WEST ) {
				carveDirection(curX, curY, DIRECTION_WEST, PASSAGE);
				curX = curX - 2;
			}
			directions.clear();			
		}
	}
}

//------------------------------------------------------------------------------
// generateRooms
//
// The room generator.  Creates candidate rooms and passes them on to
// createRoom to validate and create if valid.
//
// Notes:
//   Since rooms need to have odd width and height, this method will ensure that
//   any candidate rooms it creates will have this property.
//------------------------------------------------------------------------------
void Maze::generateRooms(int numAttempts, int minSize, int maxSize) {
	for (int i = 0; i < numAttempts; i++) {
		int x = rand() % cols;
		x = (x % 2 == 1) ? x : x + 1;
		int y = rand() % rows;
		y = (y % 2 == 1) ? y : y + 1;
		int w = rand() % (maxSize-minSize) + minSize;
		w = (w % 2 == 1) ? w : w + 1;
		int h = rand() % (maxSize-minSize) + minSize;
		h = (h % 2 == 1) ? h : h + 1;
		createRoom(x, y, w, h);
	}
}

//------------------------------------------------------------------------------
// getDirections
// 
// Given a square, returns a list of directions that contain an adjacent 
// uncarved square.
//
// Notes:
//   Used for maze generation, which requires such a list.
//------------------------------------------------------------------------------
void Maze::getDirections(vector<int> & directions, int x, int y) {
	
	if (x >= 3 && !isCarved(x-2 , y)) {
		directions.push_back(DIRECTION_WEST);
	}
	if (x <= cols - 3 && !isCarved(x+2, y)) {
		directions.push_back(DIRECTION_EAST);
	}
	if (y >= 3 && !isCarved(x, y-2)) {
		directions.push_back(DIRECTION_NORTH);
	}
	if (y <= rows -3 && !isCarved(x, y+2)) {
		directions.push_back(DIRECTION_SOUTH);
	}
}

//------------------------------------------------------------------------------
// getRoom
//
// Returns the Room specified by the room id.
//------------------------------------------------------------------------------
Room Maze::getRoom(int roomId) {
	for (vector<Room>::iterator it = rooms.begin(); it != rooms.end(); it++) {
		if (it->id == roomId) {
			return *it;
		}
	}
	
	// Just return the last room for now.  Should throw something.
	cout << "Warning - room not found!" << endl;
	return *(rooms.end());
}

//------------------------------------------------------------------------------
// markWalls
//
// Find 'walls' in the maze and mark them as such.
//
// Notes:
//   Walls are found by finding uncarved spaces and checking to see what's
//   around them.  If any adjacent space is carved (either as a passageway
//   or as part of a room), the uncarved space is marked as a wall.  
//   Uncarved spaces surrounded by other uncarved spaces on all sides are
//   uncarved, but are not walls.  Walls can be torn down to connect passageways
//   to other passageways or to rooms, but uncarved squares that aren't walls
//   (i.e. are embedded in unreachable areas of the maze) can't be.
//------------------------------------------------------------------------------
void Maze::markWalls(void) {
	for (int x = 0; x < cols; x++ ) {
		for (int y = 0; y < rows; y++ ) {
			// Is the square uncarved?
			if (m[y*cols + x].tag == EMPTY) {
				// Check the spot above and to the left of the current square
				if (y>0 && x>0 && (m[(y-1)*cols+(x-1)].tag == PASSAGE || m[(y-1)*cols+(x-1)].tag >= STARTING_ROOM)) {
					m[y*cols + x].tag = WALL;
				}
				// Check the spot directly above the current square
				else if (y>0 && (m[(y-1)*cols + x].tag == PASSAGE || m[(y-1)*cols + x].tag >= STARTING_ROOM)) {
					m[y*cols + x].tag = WALL;
				}
				// Check the spot above and to the right of the current square
				else if (y>0 && x<(cols-1) && (m[(y-1)*cols +(x+1)].tag == PASSAGE || m[(y-1)*cols +(x+1)].tag >= STARTING_ROOM)) {
					m[y*cols + x].tag = WALL;
				}
				// Check the spot to the left of the current square
				else if (x>0 && (m[y*cols + (x-1)].tag == PASSAGE || m[y*cols + (x-1)].tag >= STARTING_ROOM)) {
					m[y*cols + x].tag = WALL;
				}
				// Check the spot to the right of the current square
				else if (x<(cols-1) && (m[y*cols + (x+1)].tag == PASSAGE || m[y*cols + (x+1)].tag >= STARTING_ROOM)) {
					m[y*cols + x].tag = WALL;
				}
				// Check the spot below and to the left of the current square
				else if (y<(rows-1) && x>0 && (m[(y+1)*cols+(x-1)].tag == PASSAGE || m[(y+1)*cols+(x-1)].tag >= STARTING_ROOM)) {
					m[y*cols + x].tag = WALL;
				}
				// Check the spot directly below the current square
				else if (y<(rows-1) && (m[(y+1)*cols + x].tag == PASSAGE || m[(y+1)*cols + x].tag >= STARTING_ROOM)) {
					m[y*cols + x].tag = WALL;
				}
				// Check the spot below and to the right of the current square
				else if (y<(rows-1) && x<(cols-1) && (m[(y+1)*cols +(x+1)].tag == PASSAGE || m[(y+1)*cols +(x+1)].tag >= STARTING_ROOM)) {
					m[y*cols + x].tag = WALL;
				}
			}
		}
	}
}

//------------------------------------------------------------------------------
// openRoom
//
// Connects a room to an adjacent passage by tearing down a wall between the
// two.
//
// Notes:
//   Finds all edges that could connect a room to a passage.  Picks one to 
//   open, then iterates through the rest and randomly picks a tiny number 
//   of them to also open.
//------------------------------------------------------------------------------
void Maze::openRoom(Room &r) {
	vector<WallLoc> edges;
	
	// Check all north and south walls to see if the adjacent spaces are passageways
	for (int i = r.x; i < r.x + r.w; i+=2 ) {
		// Check the spot north of the top edge
		if (m[(r.y - 2) * cols + i].tag == PASSAGE) {
			edges.push_back(WallLoc(i, r.y, DIRECTION_NORTH));
		}
		// Check the spot south of the bottom edge
		if (m[(r.y + r.h + 1) * cols + i].tag == PASSAGE) {
			edges.push_back(WallLoc(i, r.y+r.h-1, DIRECTION_SOUTH));
		}		
	}
	
	// Check all east and west walls to see if the adjacent spaces are passageways
	for (int i = r.y; i < r.y + r.h; i+=2) {
		if (m[i*cols + (r.x - 2)].tag == PASSAGE) {
			edges.push_back(WallLoc(r.x, i, DIRECTION_WEST));
			
		}
		if (m[i*cols + (r.x + r.w  + 1)].tag == PASSAGE) {	
			edges.push_back(WallLoc(r.x+r.w-1, i, DIRECTION_EAST));
		}		
	}
	
	if(edges.size() > 0) {
		// Pick a random edge from the list and carve it.
		int toCarve = rand() % edges.size();
		carveDirection(edges[toCarve].x, edges[toCarve].y, edges[toCarve].direction, PASSAGE);
		// Re-mark the original source square as part of the room (since carveDirection will mark it
		// as passageway)
		m[edges[toCarve].y * cols + edges[toCarve].x].tag = r.id;
		
		edges.erase(edges.begin()+toCarve);
		// Now iterate through all edges and remove them with small probability 		
		for (int i=0 ; i<edges.size(); i++) {
			if (rand() % 1000 < 20) {
				carveDirection(edges[i].x, edges[i].y, edges[i].direction, PASSAGE);
				// Re-mark the original source square as part of the room (since carveDirection will mark it
				// as passageway)				
				m[edges[i].y * cols + edges[i].x].tag = r.id;
			}
		}		
	}	
}

//------------------------------------------------------------------------------
// placeStairs
//
// Place a set of stairs somewhere in the specified room.
//
// Notes:
//   Randomly picks spots in the room, and continues to do so until a valid 
//   placement is made.  'Valid' means all surrounding spaces are either room 
//   spaces or walls, and the space isn't already occupied by stairs.
//------------------------------------------------------------------------------
void Maze::placeStairs(int roomId, int type) {

	int x, y;
	bool valid = false;
	Room r = getRoom(roomId);
	
	do {
		// Pick a random spot in the room
		x = (rand() % r.w) + r.x;
		y = (rand() % r.h) + r.y;
		// Is the space to the right a wall or room space?
		if (m[y*cols + x + 1].tag == roomId || !isCarved(x+1, y)) {
			// Is the space to the left a wall or a room space?
			if(m[y*cols +x -1].tag == roomId || !isCarved(x-1, y)) {
				// Is the space below a wall or a room space?
				if(m[(y+1)*cols+x].tag == roomId || !isCarved(x, y+1)) {
					// Is the space above a wall or room space?
					if(m[(y-1)*cols+x].tag == roomId || !isCarved(x, y-1)) {
						// If so, good.  Now see if there's already stairs in this location
						valid = true;
						for(vector<Stair>::iterator it = stairs.begin(); it != stairs.end(); it++) {
							// If there is, start over
							if (it->x == x && it->y == y) {
								valid = false;
							}
						}
					}
				}
			}
		}
	} 
	while (valid == false);
	
	stairs.push_back(Stair(x, y, type, roomId));
}

//------------------------------------------------------------------------------
// removeDeadEnds
//
// Finds and removes all dead ends (squares carved in only 1 direction) from 
// the maze.
//
// Notes:
//   This might be a slow process due to all the looping.  The general steps
//   are:
//
//   do:
//     - Set number of dead ends filled in to 0
//     - Scan over the maze, going by 2s
//     - If not in a room:
//       - Count adjacent walls
//       - If adjacent wall count = 3
//         - Uncarve the current space and the adjacent non-wall space
//         - Increment number of dead ends filled in by 1
//   While dead ends filled in !=0
//------------------------------------------------------------------------------
void Maze::removeDeadEnds(void) {
	int numDeadEnds;
	bool carved[4];
	int numCarved;
	
	do {
		numDeadEnds = 0;
		for(int y = 1; y < rows - 1; y += 2) {
			for (int x = 1; x < cols - 1; x += 2) {
				numCarved = 0;
				carved[0] = false;
				carved[1] = false;
				carved[2] = false;
				carved[3] = false;
				if(m[y*cols + x].tag == PASSAGE && m[y*cols + x].carved == true) {
					if(m[y*cols +x -1].carved == true) { 
						carved[DIRECTION_WEST] = true;
						numCarved++;
					}
					if(m[y*cols + x + 1].carved == true) {
						carved[DIRECTION_EAST] = true; 
						numCarved++;
					}
					if(m[(y+1)*cols + x].carved == true) {
						carved[DIRECTION_SOUTH] = true;
						numCarved++;
					}
					if(m[(y-1)*cols + x].carved == true) {
						carved[DIRECTION_NORTH] = true;
						numCarved++;
					}
					if(numCarved == 1) {
						if(carved[DIRECTION_NORTH] == true) {
							uncarve(x, y);
							uncarve(x, y-1);
						}
						else if(carved[DIRECTION_SOUTH] == true) {
							uncarve(x, y);
							uncarve(x, y+1);
						}
						else if(carved[DIRECTION_EAST] == true) {
							uncarve(x, y);
							uncarve(x+1, y);
						}
						else if(carved[DIRECTION_WEST] == true) {
							uncarve(x, y);
							uncarve(x-1, y);
						}
						numDeadEnds++;
					}
				}		
			}
		}
	}
	while(numDeadEnds != 0);
}

//------------------------------------------------------------------------------
// uncarve
//
// Puts back a previously uncarved square.
//
// Notes:
//   Mainly used to remove dead ends.
//------------------------------------------------------------------------------
void Maze::uncarve(int x, int y) {
	m[y*cols + x].tag = EMPTY;
	m[y*cols + x].carved = false;
}

//------------------------------------------------------------------------------
// changeLitStatusAround
//
// Changes the light state of the maze square at (x,y), and each square
// adjacent to it.  This will turn lighting on 9 total squares on or off.
//------------------------------------------------------------------------------
void Maze::changeLitStatusAround(int x, int y, bool lit) {
	changeLitStatusAt(x-1, y-1, lit);
	changeLitStatusAt(x  , y-1, lit);
	changeLitStatusAt(x+1, y-1, lit);
	changeLitStatusAt(x-1,   y, lit);
	changeLitStatusAt(x  ,   y, lit);
	changeLitStatusAt(x+1,   y, lit);
	changeLitStatusAt(x-1, y+1, lit);
	changeLitStatusAt(x  , y+1, lit);
	changeLitStatusAt(x+1, y+1, lit);
}

//------------------------------------------------------------------------------
// changeRoomLitStatus
//
// Marks every square in a room as lit or unlit.  This also marks the room
// as visited, to enable fog of war.
//------------------------------------------------------------------------------
void Maze::changeRoomLitStatus(int room_id, bool lit) {
	int room_offset = room_id - STARTING_ROOM;
	Room r = rooms[room_offset];
	
	for(int i=r.x-1; i < r.x + r.w + 1; i++) {
		for (int j = r.y-1; j < r.y + r.h + 1; j++) {
			changeLitStatusAt(i, j, lit);
			if(lit == true) {
				m[j * cols + i].wasSeen = true;
			}
		}
	}
}

//------------------------------------------------------------------------------
// generate
//
// The primary maze generator.
//
// Notes:
//   Generating a maze includes the following steps:
//     - Placing rooms
//     - Filling in the remaining space with passages
//     - Removing dead ends so that every passageway eventually gets to a room.
//     - Opening walls between rooms and passageways.  At least one passageway
//       will be opened for every room, but some will open more
//     - Placing stairs in rooms
//------------------------------------------------------------------------------
void Maze::generate(void) {
	// Create rooms with widths and heights between 4 and 10 squares
	generateRooms(ROOM_ATTEMPTS, 4, 10);
	// Fill in the remaining space with passages
	generatePassages(1, 1);
	
	// Open at least one connection between each room and a passage
	for (vector<Room>::iterator it = rooms.begin(); it != rooms.end(); it++) {
		openRoom((*it));
	}
	
	// Clean up the maze and add stairs
	removeDeadEnds();
	markWalls();
	addStairs(NUM_STAIRS, NUM_STAIRS);
}

//------------------------------------------------------------------------------
// getRandomStair
//
// Finds and returns the coordinates of a random up or down stair.
//
// Notes:
//   Used to position the player when entering a new maze.  Players always
//   spawn on a set of stairs.
//------------------------------------------------------------------------------
vector<int> Maze::getRandomStair(int direction) {
	// The vector contains an equal number of up and down stairs.  Just pick 
	// them at random until we find one that goes the correct direction.
	while(1) {
		Stair s = stairs[rand() % stairs.size()];
		if (s.direction == direction) {
			vector<int> v;
			v.push_back(s.x);
			v.push_back(s.y);
			return v;
		}
	}
}

//------------------------------------------------------------------------------
// getRoomIdAt
//
// Returns the room id associated with a particular location in the maze.  
//
// Notes:
//   If the specified space isn't a room, returns -1.
//------------------------------------------------------------------------------
int Maze::getRoomIdAt(int x, int y) {
	int room = m[y * cols + x].tag;
	if(room < STARTING_ROOM) {
		return -1;
	}
	else {
		return room;
	}
}

//------------------------------------------------------------------------------
// getSquare
//
// Returns the Square specified by the given (x, y) coordinates.
//------------------------------------------------------------------------------
Square Maze::getSquare(int x, int y) {
	return m[y*cols+x];

}

//------------------------------------------------------------------------------
// init
//
// Initializes a maze structure, setting the status of each square and room.
//------------------------------------------------------------------------------
void Maze::init(void) {
	roomId = STARTING_ROOM;
	for (int i=0; i<rows*cols; i++) {
		Square s;
		s.tag = EMPTY;
		s.carved = false;
		s.isLit = false;
		s.wasSeen = false;
		m.push_back(s);
	}
	
	// Clear out the room and stair vectors
	rooms.clear();
	stairs.clear();
}

//------------------------------------------------------------------------------
// isCarved
//
// Returns whether a square is carved or not.
//------------------------------------------------------------------------------
bool Maze::isCarved(int x, int y) {
	return m[y*cols + x].carved;
}

//------------------------------------------------------------------------------
// isSquareLit
//
// Returns whether a square is currently lit or not.
//------------------------------------------------------------------------------
bool Maze::isSquareLit(int x, int y) {
	return m[y*cols + x].isLit;
}

//------------------------------------------------------------------------------
// print
//
// Debug - prints out a ASCII representation of the maze.
//------------------------------------------------------------------------------
void Maze::print(void) {
	for (int y=0; y<rows; y++) {
		for (int x=0; x<cols; x++) {
			if (m[y*cols + x].carved) {
				cout << " ";
			}
			else {
				cout << (char)219;
			}
		}
		cout << endl;
	}
	
	for (vector<Stair>::iterator it = stairs.begin(); it != stairs.end(); it++) {
		cout << (it->direction == STAIRS_UP ? "Up " : "Down ");
		cout << "stairs at (" << it->x << ", " << it->y << ")" << endl;
	}
}

//------------------------------------------------------------------------------
// printMemoryUsage
//
// Debug function that returns the total size used by a maze object.
//------------------------------------------------------------------------------			
void Maze::printMemoryUsage(void) {	
	cout << "Memory usage:" << endl;
	cout << "Size of maze class: " << sizeof(Maze) << " bytes" << endl;
	cout << "Size of square vector: " << m.size() * sizeof(Square) << " bytes" << endl;
	cout << "Size of room vector: " << rooms.size() * sizeof(Room) << " bytes" << endl;
	cout << "Size of stairs vector: " << stairs.size() * sizeof(Stair) << " bytes" << endl;
}

//------------------------------------------------------------------------------
// printRoomIds
//
// Debug function that lists the ids of all rooms in a maze.
//------------------------------------------------------------------------------
void Maze::printRoomIds(void) {
	for(vector<Room>::iterator it = rooms.begin(); it != rooms.end(); it++) {
		cout << "Room id is " << it->id << endl;
	}
}

//------------------------------------------------------------------------------
// stairsHere
// 
// Checks to see if there are stairs in the specified location, returning the
// type of stairs, or none if there aren't stairs there.
//------------------------------------------------------------------------------
int Maze::stairsHere(int x, int y) {
	for (vector<Stair>::iterator it = stairs.begin(); it != stairs.end(); it++) {
		if (it->x == x && it->y == y) {
			return it->direction;
		}
	}
	return NO_STAIRS;
}

//------------------------------------------------------------------------------
// wasSeen
//
// Returns whether the specified square has been visited by the player at some
// point. 
//------------------------------------------------------------------------------
bool Maze::wasSeen(int x, int y) {
	return m[y * cols + x].wasSeen;
}