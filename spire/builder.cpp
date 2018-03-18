#include "builder.h"

// build the generic map layout
void buildMap(map *m, int lev_num)
{
    initMapCells(m,WALL_DESTRUCT);
    carveMaze(m,lev_num);

    if (lev_num < 30)
    {
        if (ROLL(15) == 0)
            addSatellite(m,lev_num);
        if (ROLL(15) == 0)
            addVault(m,lev_num);
        if (ROLL(15) == 0)
            addCasino(m,lev_num);
        addDealerHideouts(m,lev_num);
    }

    //addDoors(m);
}
/*
void addDoors(map *m)
{
    std::vector<point> door_locs;

    for (int x = 3; x < MAPWID - 3; ++x)
    for (int y = 3; y < MAPHGT - 3; ++y)
        if (numTerrainTilesAdj(m,point(x,y),FLR_GEN,1,true) == 2 &&
            numTerrainTilesAdj(m,point(x,y),WALL_INDESTRUCT,1,false) + numTerrainTilesAdj(m,point(x,y),WALL_DESTRUCT,1,false) <= 4 &&
            m->getTerrain(point(x,y)) == FLR_GEN && ROLL(2) == 0)
        {
            door_locs.push_back(point(x,y));
        }

    for (int i = 0; i < (int)door_locs.size(); ++i)
    {
        if (numTerrainTilesAdj(m,door_locs[i],CLOSED_UNLOCKED_DOOR,1,false) == 0 &&
            numTerrainTilesAdj(m,door_locs[i],OPEN_UNLOCKED_DOOR,1,false) == 0)
            m->setTerrain(door_locs[i],(terrain_t)(ROLL(1)+9));
    }

    door_locs.clear();
}
*/
void initMapCells(map *m, terrain_t t)
{
    // "temp" point
    point p;

    // for each cell in the map
    // x (horizontal axis)
    for (int x = 0; x < MAPWID; ++x)
        // y (vertical axis)
        for (int y = 0; y < MAPHGT; ++y)
        {
            // set temp point value to (x,y)
            p.set(x,y);
            m->setAllValuesToNIL(p);
            m->setTerrain(p,t);
        }
}

void carveMaze(map *m, int lev_num)
{
    int opening_factor = 2;

    maze mze_obj;
    mze_obj.initMaze(lev_num);
    mze_obj.build();

    for (int i = 0; i < mze_obj.getMazeSize().x(); ++i)
    {
        for (int j = 0; j < mze_obj.getMazeSize().y(); ++j)
        {
            if (mze_obj.getUnit(point(i,j)) == MAZE_FLR)
                m->setTerrain(point(i+1,j+1),FLR_GEN);
            else
            {
                if (i == 0 || j == 0 || i == mze_obj.getMazeSize().x()-1 || j == mze_obj.getMazeSize().y()-1)
                    m->setTerrain(point(i+1,j+1),WALL_DESTRUCT);
                else
                    m->setTerrain(point(i+1,j+1),WALL_INDESTRUCT);
            }
        }
    }

    mze_obj.cleanupMaze();
    for (int i = 0; i < opening_factor; ++i)
    {
        for (int x = 3; x < MAPWID - 3; ++x)
        for (int y = 3; y < MAPHGT - 3; ++y)
            if (numTerrainTilesAdj(m,point(x,y),FLR_GEN,1,false) == 7 && m->getTerrain(point(x,y)) == WALL_INDESTRUCT)
            {
                for (int xx = -1; xx <= 1; ++xx)
                for (int yy = -1; yy <= 1; ++yy)
                     if (xx == 0 || yy == 0)
                         m->setTerrain(point(x+xx,y+yy),FLR_HOTSPOT);
            }

        eliminateFlrHotspots(m,FLR_GEN);
    }
}

void carveBigRoom(map *m, int lev_num)
{
    point vsize,loc;

    point maze_size = maze_sizes_for_level[lev_num-1];

    for (int x = 2; x < maze_size.x(); ++x)
    {
        for (int y = 2; y < maze_size.y(); ++y)
        {
            m->setTerrain(point(x,y),FLR_GEN);
        }
    }

    for (int i = 0; i < 200; ++i)
    {
        if (i > 100)
            vsize = point(ROLL(1) + 2, ROLL(1) + 2);
        else
            vsize = point(ROLL(7) + 2, ROLL(7) + 2);

        loc = point((int)rollrange(2,maze_size.x()-vsize.x()-1),
                    (int)rollrange(2,maze_size.y()-vsize.y()-1));

        if (canAddRoom(m,vsize,loc))
        {
            addEnclosedRoom(m,vsize,loc,FLR_HOTSPOT,false);
        }
    }

    eliminateFlrHotspots(m,WALL_INDESTRUCT);


    for (int x = 2; x < MAPWID - 2; ++x)
    for (int y = 2; y < MAPHGT - 2; ++y)
        if (numTerrainTilesAdj(m,point(x,y),FLR_GEN,1,false) == 8)
            m->setTerrain(point(x,y),WALL_INDESTRUCT);
}

void carveCircle(map *m, point p, int r, terrain_t t, int lev_num)
{
    point maze_size = maze_sizes_for_level[lev_num-1];
    double div;
    point loc;
    for (int y = -r; y <= r; ++y)
    {
        div = sqrt((double)((r*r) - (y*y)));
        for (int x = -(int)div; x <= (int)div; ++x)
        {
            loc = point(p.x()+x,p.y()+y);
            if (inRange(loc,2,2,maze_size.x()-1,maze_size.y()-1))
            {
                m->setTerrain(loc,t);
            }
        }
    }
}

void eliminateFlrHotspots(map * m, terrain_t t)
{
    for (int x = 1; x < MAPWID - 1; ++x)
    for (int y = 1; y < MAPHGT - 1; ++y)
         if (m->getTerrain(point(x,y)) == FLR_HOTSPOT)
             m->setTerrain(point(x,y),t);
}

void addSatellite(map *m, int lev_num)
{
    if (lev_num == 1)
        return;

    // 0,1,2 or 3
    int side = ROLL(3);
    int rad = 5;

    point loc;

    point maze_size = maze_sizes_for_level[lev_num-1];

    switch(side)
    {
    case(0):
        loc = point((int)rollrange(rad+2,maze_size.x()-rad),2);
        break;
    case(1):
        loc = point((int)rollrange(rad+2,maze_size.x()-rad),maze_size.y()-1);
        break;
    case(2):
        loc = point(2,(int)rollrange(rad+2,maze_size.y()-rad));
        break;
    default:
        loc = point(maze_size.x()-1,(int)rollrange(rad+2,maze_size.y()-rad));
        break;
    }

    carveCircle(m,loc,rad+2,FLR_HOTSPOT,lev_num);
    carveCircle(m,loc,rad,WALL_DESTRUCT,lev_num);
    carveCircle(m,loc,rad-2,FLR_HOTSPOT,lev_num);

    switch(side)
    {
    case(0):
        m->setTerrain(point(loc.x(),2+rad),FLR_GEN);
        m->setTerrain(point(loc.x(),2+rad-1),(terrain_t)(ROLL(1)+9));
        m->setTerrain(point(loc.x(),2),SATELLITE_TILE);
        m->setTerrain(point(loc.x(),4),SATELLITE_SPAWN_TILE);
        break;
    case(1):
        m->setTerrain(point(loc.x(),maze_size.y()-rad),(terrain_t)(ROLL(1)+9));
        m->setTerrain(point(loc.x(),maze_size.y()-1-rad),FLR_GEN);
        m->setTerrain(point(loc.x(),maze_size.y()-1),SATELLITE_TILE);
        m->setTerrain(point(loc.x(),maze_size.y()-3),SATELLITE_SPAWN_TILE);
        break;
    case(2):
        m->setTerrain(point(2+rad,loc.y()),FLR_GEN);
        m->setTerrain(point(2+rad-1,loc.y()),(terrain_t)(ROLL(1)+9));
        m->setTerrain(point(2,loc.y()),SATELLITE_TILE);
        m->setTerrain(point(4,loc.y()),SATELLITE_SPAWN_TILE);
        break;
    default:
        m->setTerrain(point(maze_size.x()-rad,loc.y()),(terrain_t)(ROLL(1)+9));
        m->setTerrain(point(maze_size.x()-1-rad,loc.y()),FLR_GEN);
        m->setTerrain(point(maze_size.x()-1,loc.y()),SATELLITE_TILE);
        m->setTerrain(point(maze_size.x()-3,loc.y()),SATELLITE_SPAWN_TILE);
        break;
    }
}

void addCasino(map *m, int lev_num)
{
    point vsize = point(9+ROLL(2)*2,9+ROLL(2)*2);

    point maze_size = maze_sizes_for_level[lev_num-1];

    point loc = point((int)rollrange(2,maze_size.x()-vsize.x()),
                      (int)rollrange(2,maze_size.y()-vsize.y()));

    point dp;

    if (canAddRoom(m,vsize,loc))
    {
        addEnclosedRoom(m,vsize,loc,FLR_CASINOMARKER,true);

        for (int x = 0; x < vsize.x(); ++x)
        for (int y = 0; y < vsize.y(); ++y)
        {
            dp = point(x+loc.x(),y+loc.y());
            if (x > 2 && y > 2 && x < vsize.x() - 3 && y < vsize.y() - 3)
            if (x % 2 == 1 && y % 2 == 1)
                m->setTerrain(dp,FLR_MACHINEMARKER);
        }

        point door_loc;

        if (ROLL(1) == 0)
            door_loc = point(ROLL(vsize.x()-5)+2,ROLL(1)*(vsize.y()-3)+1);
        else
            door_loc = point(ROLL(1)*(vsize.x()-3)+1,ROLL(vsize.y()-5)+2);

        m->setTerrain(point(door_loc.x() + loc.x(),door_loc.y() + loc.y()),(terrain_t)(ROLL(1)+9));
    }
}

// . . . . . . . . .
// . # # # # # # # .
// . # . . . . . # .
// . # . $ . $ . # .
// . # . . . . . # .
// . # . $ . $ . # .
// . # . . . . . # .
// . # # # + # # # .
// . . . . . . . . .

void addVault(map *m, int lev_num)
{
    point vsize = point(8+ROLL(4),8+ROLL(4));

    point maze_size = maze_sizes_for_level[lev_num-1];

    point loc = point((int)rollrange(2,maze_size.x()-vsize.x()),
                      (int)rollrange(2,maze_size.y()-vsize.y()));

    if (canAddRoom(m,vsize,loc))
    {
        addEnclosedRoom(m,vsize,loc,FLR_VAULTMARKER,true);

        point door_loc;

        if (ROLL(1) == 0)
            door_loc = point(ROLL(vsize.x()-5)+2,ROLL(1)*(vsize.y()-3)+1);
        else
            door_loc = point(ROLL(1)*(vsize.x()-3)+1,ROLL(vsize.y()-5)+2);

        m->setTerrain(point(door_loc.x() + loc.x(),door_loc.y() + loc.y()),CLOSED_UNLOCKED_DOOR);
    }
}

void addDealerHideouts(map *m, int lev_num)
{
    int num_dealers = ROLL(max_dealers_per_level[lev_num - 1]);

    point loc, maze_size;

    for (int i = 0; i < num_dealers; ++i)
    {
        if (ROLL(2) == 0)
        {
            maze_size = maze_sizes_for_level[lev_num-1];

            loc = point(rollrange(2,maze_size.x()-6),
                        rollrange(2,maze_size.y()-6));

            if (canAddRoom(m,point(6,6),loc))
            {
                addSingleDealerHideout(m,loc);
            }
        }
    }
}

void addEnclosedRoom(map *m, point rsize, point loc, terrain_t inside_cell, bool add_edges)
{
    point dp;

    for (int x = 0; x < rsize.x(); ++x)
    for (int y = 0; y < rsize.y(); ++y)
    {
        dp.set(x+loc.x(),y+loc.y());
        if(inRange(point(x,y),2,2,rsize.x()-3,rsize.y()-3) || !add_edges)
            m->setTerrain(dp,inside_cell);
        else if (x == 0 || y == 0 || x == rsize.x() - 1 || y == rsize.y() - 1)
            m->setTerrain(dp,FLR_GEN);
        else
            m->setTerrain(dp,WALL_DESTRUCT);
    }
}

void addSingleDealerHideout(map * m, point loc)
{
    addEnclosedRoom(m,point(6,6),loc,FLR_HOTSPOT,true);

    point door_loc;

    point dealer_loc;

    if (ROLL(1) == 0)
        door_loc = point(ROLL(1)+2,ROLL(1)*3 + 1);
    else
        door_loc = point(ROLL(1)*3 + 1,ROLL(1)+2);

    m->setTerrain(point(door_loc.x() + loc.x(),door_loc.y() + loc.y()),(terrain_t)(ROLL(1)+9));

    if (door_loc.x() == 1)
        dealer_loc = point(3,ROLL(1)+2);
    else if (door_loc.x() == 4)
        dealer_loc = point(2,ROLL(1)+2);
    else if (door_loc.y() == 1)
        dealer_loc = point(ROLL(1)+2,3);
    else
        dealer_loc = point(ROLL(1)+2,2);

    m->setTerrain(point(dealer_loc.x() + loc.x(),dealer_loc.y() + loc.y()),FLR_DEALERMARKER);
}

// example:

// . . . . . .
// . # # # # .
// . # @ . + .
// . # . . # .
// . # # # # .
// . . . . . .

bool canAddRoom(map *m, point rsize, point loc)
{
    point flr_loc;

    for (int x = -1; x <= rsize.x(); ++x)
    for (int y = -1; y <= rsize.y(); ++y)
    {
        flr_loc.set(loc.x()+x,loc.y()+y);
        if (!inRange(flr_loc,2,2,MAPWID-3,MAPHGT-3))
            return false;
        if (m->getTerrain(flr_loc) == FLR_VAULTMARKER ||
            m->getTerrain(flr_loc) == FLR_DEALERMARKER ||
            m->getTerrain(flr_loc) == FLR_CASINOMARKER||
            m->getTerrain(flr_loc) == FLR_MACHINEMARKER ||
            m->getTerrain(flr_loc) == FLR_HOTSPOT)
            return false;
    }

    return true;
}

maze::maze()
{
}

// create grid full of trapped open cells (see header for visual)
void maze::initMaze(int lev_num)
{
    maze_size = maze_sizes_for_level[lev_num-1];
    maze_array.resize(maze_size.x(), VECTOR<maze_unit_type>(maze_size.y(), MAZE_WALL));

    for (int x = 0; x < maze_size.x(); ++x)
    for (int y = 0; y < maze_size.y(); ++y)
    {
        if (x % 2 != 0 && y % 2 != 0)
            maze_array[x][y] = MAZE_FLR;
        else
            maze_array[x][y] = MAZE_WALL;
    }

    maze_array[0][0] = MAZE_WALL;
}

void maze::cleanupMaze()
{
    VECTOR < VECTOR < maze_unit_type > >().swap(maze_array);
}

int maze::getGuaranteedNumMazeFloors()
{
    return (int) ( ( maze_size.x() - 1) / 2) * (int) ( ( maze_size.y() - 1) / 2);
}

void maze::digIntoMazeWall(maze_dig_direction &direction,
                           point &selected, point &knock,
                           point u, point d, point l, point r)
{
    while (!isUnitTrapped(selected))
    {
        direction = pickDigDirection();

        if (direction == DIG_UP && u.y() > 0)
        {
            selected = knock = u;
            knock.sety(u.y() + 1);
        }
        else if (direction == DIG_DOWN && d.y() < maze_size.y() - 1)
        {
            selected = knock = d;
            knock.sety(d.y() - 1);
        }
        else if (direction == DIG_LEFT && l.x() > 0)
        {
            selected = knock = l;
            knock.setx(l.x() + 1);
        }
        else if (direction == DIG_RIGHT && r.x() < maze_size.x() - 1)
        {
            selected = knock = r;
            knock.setx(r.x() - 1);
        }
    }
}

void maze::setAdjacentPoints(point current_dig, point &u, point &d, point &l, point &r)
{
    l.set(current_dig.x() - 2,current_dig.y());
    r.set(current_dig.x() + 2,current_dig.y());
    u.set(current_dig.x(),current_dig.y() - 2);
    d.set(current_dig.x(),current_dig.y() + 2);
}

bool maze::anyAdjacentTrapped(point current_dig, point u, point d, point l, point r)
{
    return (isUnitTrapped(u) && u.y() > 0) || (isUnitTrapped(d) && d.y() < maze_size.y() - 1) ||
           (isUnitTrapped(l) && l.x() > 0) || (isUnitTrapped(r) && r.x() < maze_size.x() - 1);
}

// use stack method to create maze (one path solution)
void maze::build()
{
    std::stack <point> unit_stack;

    point current_dig = point(ROLL((int)( (maze_size.x() - 1) / 2) - 1) * 2 + 1,ROLL((int)( (maze_size.y() - 1) / 2) - 1) * 2 + 1);

    start_dig = current_dig;

    int num_visited = 1;

    point selected, knock, new_dig, u, d, l, r;

    maze_dig_direction direction;

    while (num_visited < getGuaranteedNumMazeFloors() )
    {
        setAdjacentPoints(current_dig,u,d,l,r);

        if (anyAdjacentTrapped(current_dig,u,d,l,r))
        {
            digIntoMazeWall(direction,selected,knock,u,d,l,r);

            maze_array[knock.x()][knock.y()] = MAZE_FLR;

            unit_stack.push(current_dig);

            current_dig = selected;

            num_visited++;
        }
        else
        {
            new_dig = unit_stack.top();

            unit_stack.pop();

            current_dig = new_dig;
        }
    }
}

// test for trapped cell (used by build function for determining whether or
// not we should dig here.
bool maze::isUnitTrapped(point p)
{
    point a;

    for (int x = p.x() - 1; x <= p.x() + 1; ++x)
    for (int y = p.y() - 1; y <= p.y() + 1; ++y)
    {
        a.set(x,y);

        if (x != p.x() || y != p.y())
        if (inRange(a,0,0,maze_size.x()-1,maze_size.y()-1))
        if (maze_array[x][y] != MAZE_WALL)
            return false;

    }
    return true;
}

maze_dig_direction maze::pickDigDirection()
{
    if (ROLL(3) == 0)
        return (maze_dig_direction)ROLL(1);
    return (maze_dig_direction)(ROLL(1)+2);
}

// get unit
maze_unit_type maze::getUnit(point p)
{
    return maze_array[p.x()][p.y()];
}

// set unit to...
void maze::setUnit(point p, maze_unit_type mut)
{
    maze_array[p.x()][p.y()] = mut;
}

point maze::getMazeSize()
{
    return maze_size;
}
