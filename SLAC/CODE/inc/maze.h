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
#ifndef __MAZE_H__
#define __MAZE_H__
#include <vector>

using namespace std;

const int DIRECTION_NORTH = 0;
const int DIRECTION_SOUTH = 1;
const int DIRECTION_EAST = 2;
const int DIRECTION_WEST = 3;

const int EMPTY = 0;
const int PASSAGE = 1;
const int WALL = 2;

const int STARTING_ROOM = 100;

const int ROOM_ATTEMPTS = 200;

const int NO_STAIRS = -1;
const int STAIRS_UP = 0;
const int STAIRS_DOWN = 1;
const int NUM_STAIRS = 3;

struct Stair {
    int x;
	int y;
	int direction;
	int roomId;
	Stair(int x, int y, int direction, int roomId) : x(x), y(y), direction(direction), roomId(roomId) {}
};

struct WallLoc {
	int x;
	int y;
	int direction;
	WallLoc(int x, int y, int direction) : x(x), y(y), direction(direction) {}
};

struct Room {
	int id;
	int x;
	int y;
	int w;
	int h;
	Room(int id, int x, int y, int w, int h) : id(id), x(x), y(y), w(w), h(h) {}
};

struct Square {
	int tag;			// Contains the ID of the room it sits in, if any
	bool carved;		// Is the square solid or has it been carved
	bool isLit;			// Is the square currently lit?
	bool wasSeen;		// The player has previously seen/lit this location but may or may not be in it now
};

class Maze {
private:
	vector<Square> m;
	vector<Room> rooms;
	vector<Stair> stairs;	
	int rows;
	int cols;
    int roomId;

	void carve(int x, int y, int tag);
	void uncarve(int x, int y);
	void carveDirection(int x, int y, int direction, int tag);
	bool createRoom(int x, int y, int w, int h);
	void addStairs(int numUpStairs, int numDownStairs);
	void generateRooms(int numAttempts, int minSize, int maxSize);
	void generatePassages(int x, int y);
	void getDirections(vector<int> & directions, int x, int y);
	void openRoom(Room &r);
	void removeDeadEnds(void); 
	Room getRoom(int roomId);
	void placeStairs(int roomId, int type);
	void markWalls(void);	
	void changeLitStatusAt(int x, int y, bool lit);
	
public:
	Maze(int x, int y);
	void init(void);
	void print(void);
	int getWidth(void) { return cols; }
	int getHeight(void) { return rows; }
	Square getSquare(int x, int y);
	bool isCarved(int x, int y);
	bool isSquareLit(int x, int y);
	bool isSquareRoomLit(int x, int y);
	bool wasSeen(int x, int y);
	int getRoomIdAt(int x, int y);
	void generate(void);
	void printMemoryUsage(void); 
	void printRoomIds(void);
	int stairsHere(int x, int y);
	vector<int> getRandomStair(int direction);
	void changeLitStatusAround(int x, int y, bool lit);	
	void changeRoomLitStatus(int room, bool lit);

};
#endif
