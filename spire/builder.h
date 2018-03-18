#ifndef BUILDER_H_
#define BUILDER_H_

#include "map.h"
#include "misc.h"
#include <stack>

static const int max_dealers_per_level[30] =
{
    1,1,1,1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,3,3,3,3,3,3,3,3,3,3
};

static const point maze_sizes_for_level[30] =
{
    point(21,17),
    point(23,17),
    point(25,17),
    point(25,19),
    point(25,21),
    point(27,21),
    point(29,21),
    point(31,21),
    point(33,21),
    point(35,21),
    point(37,21),
    point(39,21),
    point(41,21),
    point(43,21),
    point(45,21),
    point(47,21),
    point(49,21),
    point(51,21),
    point(53,21),
    point(53,21),
    point(53,21),
    point(53,21),
    point(53,21),
    point(53,21),
    point(53,21),
    point(53,21),
    point(53,21),
    point(53,21),
    point(53,21),
    point(53,21)
};

enum maze_unit_type
{
    MAZE_FLR,
    MAZE_WALL
};

enum maze_dig_direction
{
    DIG_UP,
    DIG_DOWN,
    DIG_LEFT,
    DIG_RIGHT
};

void buildMap(map *, int);
void addDoors(map *);
//void addGiantBoxes(map *);
void carveBigRoom(map *,int);
void carveMaze(map *,int);
void initMapCells(map *, terrain_t);
void carveCircle(map *, point, int, terrain_t, int);
//void haveDiggersDig(map *, point, point, int, int);
bool canAddRoom(map *, point, point);
void addSingleDealerHideout(map *,point);
void addDealerHideouts(map *,int);
void addSatellite(map *, int);
void addCasino(map *,int);
void addVault(map *,int);
void addEnclosedRoom(map *,point,point,terrain_t,bool);
void eliminateFlrHotspots(map*,terrain_t);
// VN stands for Von Neumann (up down left right)
//int terrainTilesAdjVN(map *, point, terrain_t);
//void makeCircularRooms(map *, int);

// maze is a two dimensional array of 0s and 1s. Initalizes as:
// 111111111.....
// 101010101.....
// 111111111.....
// 101010101.....
// 111111111.....
// .........
// .........
// .........
// Call to "build()" digs random maze with one path solution such as:
// 111111111
// 1S0000001
// 111010101
// 1000101F1
// 111111111
// Where the maze can only be solved in one way from S to F without passing through
// tiles more than once...

class maze
{
    public:
        // default const. to initialize all values of mazeArray to 0
        maze();
        // init maze to be trapped 0 (floor) tiles
        void initMaze(int);
        // create maze
        void build();
        // set unit at location
        void setUnit(point, maze_unit_type);
        // dig...
        void digIntoMazeWall(maze_dig_direction&, point&, point&, point, point, point, point);
        // set points adjacent to "current_dig" in build()
        void setAdjacentPoints(point, point&, point&, point&, point&);
        void cleanupMaze();
        // check if any points adjacent to "current_dig" are trapped
        bool anyAdjacentTrapped(point, point, point, point, point);
        // get num 0 tiles adjacent
        int flrTilesAdj(point);
        // get num MAZE_FLOOR units at initialization
        int getGuaranteedNumMazeFloors();
        // check if unit at point is trapped by 1s
        bool isUnitTrapped(point);
        // randomly pick dig direction
        maze_dig_direction pickDigDirection();
        // get unit at ith location
        maze_unit_type getUnit(point);
        point getMazeSize();
    private:
        // maze array value designations:
        VECTOR < VECTOR < maze_unit_type > > maze_array;
        // start point from maze dig algorithm
        point start_dig;
        // end point from maze dig algorithm
        point finish_dig;
        point maze_size;
        // maze RNG
};

#endif
