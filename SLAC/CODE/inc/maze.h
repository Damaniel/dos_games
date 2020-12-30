#ifndef __MAZE_H__
#define __MAZE_H__
#include <vector>
#include <iostream>
#include <stack>
#include <cstdlib>
#include <ctime>

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
	bool isLit;		// Used to optimize room lighting, avoiding relighting of rooms that already are
	Room(int id, int x, int y, int w, int h, bool isLit) : id(id), x(x), y(y), w(w), h(h), isLit(isLit) {}
};

struct Square {
	int tag;
	bool carved;
	bool isLit;
	bool isRoomLit;		// This square has been lit as part of a room, so it shouldn't be turned off by the player walking around
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
	void changeRoomLitStatusAt(int x, int y, bool roomLit);
	void changeLitStatusAtIfNonRoomLit(int x, int y, bool lit);
	
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
	bool isRoomLit(int roomId);
	int getRoomIdAt(int x, int y);
	void generate(void);
	void printMemoryUsage(void); 
	void printRoomIds(void);
	int stairsHere(int x, int y);
	vector<int> getRandomStair(int direction);
	void changeLitStatusAround(int x, int y, bool lit);	
	void changeLitStatusAroundPlayer(int x, int y, bool lit);
	void changeRoomLitStatus(int room, bool lit);

};
#endif
