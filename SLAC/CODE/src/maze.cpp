#include "maze.h"
#include <vector>
#include <iostream>
#include <stack>
#include <algorithm>
#include <cstdlib>
#include <ctime>

/*
 * Constructor.
 */
Maze::Maze(int x, int y) {
	/* If the maze is of even size, add an additional row/column to ensure 
	 * a border surrounds the entire maze. */
	rows = y % 2 == 0 ? y + 1 : y;
	cols = x % 2 == 0 ? x + 1 : x;
	init();
}

/*
 * Reset the state of the maze, uncarving all squares, and removing 
 * all rooms and stairs.
 */
void Maze::init(void) {
	roomId = STARTING_ROOM;
	for (int i=0; i<rows*cols; i++) {
		Square s;
		s.tag = EMPTY;
		s.carved = false;
		s.isLit = true;
		m.push_back(s);
	}
	
	// Clear out the room and stair vectors
	rooms.clear();
	stairs.clear();
}

/*
 * Debug - prints out a ASCII representation of the maze.
 */
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

void Maze::addStairs(int numUpStairs, int numDownStairs) {
	// Adds the specified number of up and down stairs to the maze.
	// Some notes:
	//  - More than one set of stairs can't be in the same room
	//  - If there are more combined up and down stairs than rooms
	//  - Stairs can only be generated in rooms
	//  - The location that stairs can be placed must be entirely surrounded
	//    by either room spaces or walls.
	
	int numRooms = roomId - STARTING_ROOM - 1;
	
	if (numUpStairs + numDownStairs > numRooms) {
		cout << "not enough rooms!" << endl;
		return;
	}
	
	vector<int> selectedRooms;
	vector<int>::iterator roomIt;
	
	for (int i=0; i < numUpStairs; i++) {
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
		placeStairs(candidateRoom, STAIRS_UP);
		selectedRooms.push_back(candidateRoom);
	}
	
	
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

void Maze::placeStairs(int roomId, int type) {
	// Randomly pick spots in the room, and continue to do so until a valid placement
	// is made.  'Valid' means all surrounding spaces are either room spaces or walls,
	// and the space isn't already occupied by stairs
	int x, y;
	bool valid = false;
	Room r = getRoom(roomId);
	
	do {
		x = (rand() % r.w) + r.x;
		y = (rand() % r.h) + r.y;
		if (m[y*cols + x + 1].tag == roomId || !isCarved(x+1, y)) {
			if(m[y*cols +x -1].tag == roomId || !isCarved(x-1, y)) {
				if(m[(y+1)*cols+x].tag == roomId || !isCarved(x, y+1)) {
					if(m[(y-1)*cols+x].tag == roomId || !isCarved(x, y-1)) {
						valid = true;
						for(vector<Stair>::iterator it = stairs.begin(); it != stairs.end(); it++) {
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

Square Maze::getSquare(int x, int y) {
	return m[y*cols+x];

}

/*
 * Carves a square out of the maze.
 */
void Maze::carve(int x, int y, int tag) {
	m[y*cols + x].tag = tag;
	m[y*cols + x].carved = true;
}

/*
 * Puts back a previously carved square.
 */
void Maze::uncarve(int x, int y) {
	m[y*cols + x].tag = EMPTY;
	m[y*cols + x].carved = false;
}

/*
 * Carves the maze in a particular direction.  Due to the way that mazes are
 * stored, carving in a direction requires carving three squares - the source square,
 * the target square, and the square between them.
 */
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

/*
 * Helper function - returns whether a square is carved or not.
 */
bool Maze::isCarved(int x, int y) {
	return m[y*cols + x].carved;
}

/*
 * Helper function - generates a list of directions from a given square,
 * where each direction is connected to an uncarved square.
 */
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

/* 
 * Attempts to add a room to the maze.  
 */
bool Maze::createRoom(int x, int y, int w, int h) {
	
	/* To fit in the maze, the room must:
		- be odd in height and width
		- be offset by an odd amount
		- fit within the bounds of the room
		
	   If any of these aren't true, the room won't be created.
	*/
	if (x % 2 == 0 || y % 2 == 0) {
		return false;
	}
	if (w % 2 == 0 || h % 2 == 0) {
		return false;
	}
	
	// The room can't be pressed against any edge of the maze (it makes generation of
	// passages more consistent)
	if (x <3 || y < 3 || (x + w) > (cols - 3) || (y + h) > (rows - 3)) {
		return false;
	}
	
	// Check for overlap with existing space.  If there is any, don't make the room
	for (int i = x; i < x+w; i++ ) {
		for (int j = y; j < y+h; j++ ) {
			if (m[j*cols + i].tag >= STARTING_ROOM) {
				return false;
			}
		}
	}
		
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

/* 
 * Generates a maze.  Uses the upper left corner as the starting point. 
 */
void Maze::generate(void) {
	generateRooms(ROOM_ATTEMPTS, 4, 10);
	generatePassages(1, 1);
	
	for (vector<Room>::iterator it = rooms.begin(); it != rooms.end(); it++) {
		openRoom((*it));
	}
	
	removeDeadEnds();
	markWalls();
	addStairs(NUM_STAIRS, NUM_STAIRS);
}

/* 
 * Finds all edges that connect a room to a passage.  Picks one to open, then iterates
 * through the rest and randomly picks a tiny number of them to also open.
 */ 
void Maze::openRoom(Room &r) {
	vector<WallLoc> edges;
	
	//cout << "Room info - id: " << r.id << " (" << r.x << ", " << r.y << "), ";
	//cout << "width = " << r.w << ", height = " << r.h << endl;
	
	// Check all north and south walls to see if the adjacent spaces are passageways
	for (int i = r.x; i < r.x + r.w; i+=2 ) {
		// Check the spot north of the top edge
		if (m[(r.y - 2) * cols + i].tag == PASSAGE) {
			//cout << "Adding NORTH wall at (" << i << ", " << r.y << ")" << endl;
			edges.push_back(WallLoc(i, r.y, DIRECTION_NORTH));
		}
		// Check the spot south of the bottom edge
		if (m[(r.y + r.h + 1) * cols + i].tag == PASSAGE) {
			//cout << "Adding SOUTH wall at (" << i << ", " << r.y+r.h-1 << ")" << endl;
			edges.push_back(WallLoc(i, r.y+r.h-1, DIRECTION_SOUTH));
		}		
	}
	
	// Check all east and west walls to see if the adjacent spaces are passageways
	for (int i = r.y; i < r.y + r.h; i+=2) {
		if (m[i*cols + (r.x - 2)].tag == PASSAGE) {
			//cout << "Adding WEST wall at (" << r.x << ", " << i << ")" << endl;
			edges.push_back(WallLoc(r.x, i, DIRECTION_WEST));
			
		}
		if (m[i*cols + (r.x + r.w  + 1)].tag == PASSAGE) {
			//cout << "Adding EAST wall at (" << r.x+r.w-1 << ", " << i << ")" << endl;		
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

/* 
 * Attempts to create rooms of a specified size in random locations.  Not all rooms that are attempted
 * to be placed will be.  Those that are will be added to a vector of rooms for further processing
 * down the line. 
 */
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

/* 
 * Uses a recursive backtracker to generate a maze.  It starts by carving a maze as greedily
 * as possible, until there are no adjacent squares to carve to.  Then, the maze is systematically
 * scanned over, and any uncarved spaces that are adjacent to carved ones will be attached to one of
 * the adjacent carved spaces.  The result is that every space in the maze should be connected. 
 */
void Maze::generatePassages(int x, int y) {
	int curX = x;
	int curY = y;
	int direction;
	bool done = false;
	vector<int> directions;
	stack< pair<int, int> > visited;
	
	while (!done) {
		pair<int, int> cur = make_pair(curX, curY);
		
		getDirections(directions, curX, curY);
		if (directions.size() == 0) {
			if (visited.size() == 0) {
				done = true;
			}
			else {
				pair<int, int> top = visited.top();
				visited.pop();
				curX = top.first;
				curY = top.second;
			}
		}
		else {
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

/* 
 * Finds and removes all dead ends (squares carved in only 1 direction) from 
 * the maze.
 */
void Maze::removeDeadEnds(void) {

	// This might be a slow process due to all the looping:
	// Do:
	//   - Set number of dead ends filled in to 0
	// 	 - Scan over the maze, going by 2s
	//   - If not in a room:
	//     - Count adjacent walls
	//     - If adjacent wall count = 3
	//       - Uncarve the current space and the adjacent non-wall space
	//       - Increment number of dead ends filled in by 1
	// While dead ends filled in !=0
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

int Maze::stairsHere(int x, int y) {
	for (vector<Stair>::iterator it = stairs.begin(); it != stairs.end(); it++) {
		if (it->x == x && it->y == y) {
			return it->direction;
		}
	}
	return NO_STAIRS;
}

void Maze::markWalls(void) {
	// Check all 8 spaces around each square tagged EMPTY.  If any of the spaces
	// are marked PASSAGE or are part of a room, then mark that space as a WALL.  
	for (int x = 0; x < cols; x++ ) {
		for (int y = 0; y < rows; y++ ) {
			if (m[y*cols + x].tag == EMPTY) {
				if (y>0 && x>0 && (m[(y-1)*cols+(x-1)].tag == PASSAGE || m[(y-1)*cols+(x-1)].tag >= STARTING_ROOM)) {
					m[y*cols + x].tag = WALL;
				}
				else if (y>0 && (m[(y-1)*cols + x].tag == PASSAGE || m[(y-1)*cols + x].tag >= STARTING_ROOM)) {
					m[y*cols + x].tag = WALL;
				}
				else if (y>0 && x<(cols-1) && (m[(y-1)*cols +(x+1)].tag == PASSAGE || m[(y-1)*cols +(x+1)].tag >= STARTING_ROOM)) {
					m[y*cols + x].tag = WALL;
				}
				else if (x>0 && (m[y*cols + (x-1)].tag == PASSAGE || m[y*cols + (x-1)].tag >= STARTING_ROOM)) {
					m[y*cols + x].tag = WALL;
				}
				else if (x<(cols-1) && (m[y*cols + (x+1)].tag == PASSAGE || m[y*cols + (x+1)].tag >= STARTING_ROOM)) {
					m[y*cols + x].tag = WALL;
				}
				else if (y<(rows-1) && x>0 && (m[(y+1)*cols+(x-1)].tag == PASSAGE || m[(y+1)*cols+(x-1)].tag >= STARTING_ROOM)) {
					m[y*cols + x].tag = WALL;
				}
				else if (y<(rows-1) && (m[(y+1)*cols + x].tag == PASSAGE || m[(y+1)*cols + x].tag >= STARTING_ROOM)) {
					m[y*cols + x].tag = WALL;
				}
				else if (y<(rows-1) && x<(cols-1) && (m[(y+1)*cols +(x+1)].tag == PASSAGE || m[(y+1)*cols +(x+1)].tag >= STARTING_ROOM)) {
					m[y*cols + x].tag = WALL;
				}
			}
		}
	}
}
					
			
/*
 * Debug function.  Dumps memory info.
 */
void Maze::printMemoryUsage(void) {	
	cout << "Memory usage:" << endl;
	cout << "Size of maze class: " << sizeof(Maze) << " bytes" << endl;
	cout << "Size of square vector: " << m.size() * sizeof(Square) << " bytes" << endl;
	cout << "Size of room vector: " << rooms.size() * sizeof(Room) << " bytes" << endl;
}

// Returns the coordinates of a random stair that goes the specified direction (up or down)
vector<int> Maze::getRandomStair(int direction) {
	// The vector contains an equal number of up and down stairs.  Just pick them at random
	// until we find one that goes the correct direction.
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
