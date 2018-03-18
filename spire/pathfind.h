#ifndef PATHFIND_H_
#define PATHFIND_H_

#define INFINITE 999999

#include "map.h"

static const point pfind_retval_priority[8] =
{
    point(1,0),
    point(-1,0),
    point(0,-1),
    point(0,1),
    point(-1,-1),
    point(1,-1),
    point(-1,1),
    point(1,1)
};

class pathfind
{
    public:
        pathfind();
        point dijkstra(map *, point, point);
        point getMinDistPoint(point);
        void computeDistance(point);
        void initPathfind(map *, point, point);
    private:
        int dist_val[MAPHGT][MAPWID];
        bool has_ended;
        bool too_far;
        point min_scan;
        point max_scan;
        int curr_min_dist;
        int iters;
};

#endif
