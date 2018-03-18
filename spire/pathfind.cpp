#include "pathfind.h"

pathfind::pathfind()
{
}

point pathfind::dijkstra(map *m, point source, point sink)
{
    if (!inRange(source,1,1,MAPWID-1,MAPHGT-1) ||
        !inRange(sink,1,1,MAPWID-1,MAPHGT-1))
        return source;

    if (isAt(source,sink))
        return source;

    if (isCellBlocking(m->getTerrain(sink)))
        return source;

    curr_min_dist = INFINITE;

    initPathfind(m,source,sink);

    iters = 0;

    do
    {
        computeDistance(source);

        if (has_ended)
            break;

    }
    while(1);

    return getMinDistPoint(source);
}

point pathfind::getMinDistPoint(point p)
{
    point ret_val = point(1,1);

    curr_min_dist = INFINITE;

    point offset;

    point adj;

    for (int i = 0; i < 8; ++i)
    {
        offset = pfind_retval_priority[i];
        adj.set(p.x() + offset.x(), p.y() + offset.y());
        if (dist_val[adj.y()][adj.x()] > -1)
        if (dist_val[adj.y()][adj.x()] < curr_min_dist)
        {
            curr_min_dist = dist_val[adj.y()][adj.x()];
            ret_val.set(adj.x(), adj.y());
        }
    }

    if (curr_min_dist == INFINITE)
        return p;

    return ret_val;
}

void pathfind::computeDistance(point source)
{
    point adj;
    bool reached_source = false;
    point new_minscan = min_scan;
    point new_maxscan = max_scan;

    has_ended = true;

    int px,py,dx,dy;
    point loc;

    for (px = min_scan.x(); px <= max_scan.x(); ++px)
    for (py = min_scan.y(); py <= max_scan.y(); ++py)
    {
        iters++;
        if (dist_val[py][px] > -1)
        {
            curr_min_dist = INFINITE;
            for (dx = -1; dx <= 1; ++dx)
            for (dy = -1; dy <= 1; ++dy)
            {
                if (dx != 0 || dy != 0)
                {
                    adj.set(dx+px,dy+py);
                    if (dist_val[adj.y()][adj.x()] > -1)
                    if (dist_val[adj.y()][adj.x()] < curr_min_dist)
                        curr_min_dist = dist_val[adj.y()][adj.x()];
                }
            }
            if (dist_val[py][px] >= 2 + curr_min_dist)
            {
                dist_val[py][px] = curr_min_dist + 1;
                has_ended = false;

                if (px == min_scan.x())
                    new_minscan.setx(px-1);
                if (py == min_scan.y())
                    new_minscan.sety(py-1);
                if (px == max_scan.x())
                    new_maxscan.setx(px+1);
                if (py == max_scan.y())
                    new_maxscan.sety(py+1);

                loc.set(px,py);

                if (pointDist(source,loc) <= 1)
                {
                    reached_source = true;
                }
            }
        }
    }

    if (reached_source)
        has_ended = true;

    min_scan = point((int)MAXIMUM(2,new_minscan.x()),(int)MAXIMUM(2,new_minscan.y()));
    max_scan = point((int)MINIMUM(MAPWID-3,new_maxscan.x()),(int)MINIMUM(MAPHGT-3,new_maxscan.y()));
}


// ? ? ? ?
// 2 2 # 2
// 2 1 1 1
// 2 1 0 #
// # # 1 1

void pathfind::initPathfind(map *m, point source, point sink)
{
    point p;

    bool b;

    int y,x;

    min_scan.set(sink.x()-1,sink.y()-1);
    max_scan.set(sink.x()+1,sink.y()+1);

    for (y = 0; y < MAPHGT; ++y)
    for (x = 0; x < MAPWID; ++x)
    {
        p.set(x,y);

        b = isPathBlocking(m->getCell(p));

        if (!isAt(p,sink))
        {
            if (!b)
                dist_val[y][x] = INFINITE;
            else
                dist_val[y][x] = -1;
        }
        else
            dist_val[y][x] = 0;
    }
}
