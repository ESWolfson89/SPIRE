#include "bresenham.h"


integer_line::integer_line()
{
}

int integer_line::getLineSize()
{
    return points.size();
}

void integer_line::clearPoints()
{
    points.clear();
}

bool integer_line::validLinePoint(point source)
{
    return inRange(source,0,0,MAPWID-1,MAPHGT-1);
}

void integer_line::checkAllBoundaries()
{
    int line_size = (int)getLineSize();

    for (int i = 0; i < line_size; ++i)
    {
        if (!validLinePoint(points[0]))
            points.erase(points.begin());
        else
            break;
    }
}

void integer_line::bresenham(point a, point b)
{
    clearPoints();

    if (!validLinePoint(a) || isAt(a,b))
        return;

    point dp = point(abs(a.x()-b.x()),abs(a.y()-b.y()));

    point sp;

    if (b.x() < a.x())
        sp.setx(1);
    else
        sp.setx(-1);

    if (b.y() < a.y())
        sp.sety(1);
    else
        sp.sety(-1);

    int error = dp.x() - dp.y();
    int error2;

    while (!isAt(a,b))
    {
        points.push_back(b);
        error2 = 2 * error;
        if (error2 > -dp.y())
        {
            error -= dp.y();
            b.setx(b.x() + sp.x());
        }
        if (error2 < dp.x())
        {
            error += dp.x();
            b.sety(b.y() + sp.y());
        }
    }
    points.push_back(b);

    checkAllBoundaries();
}

bool integer_line::isBlocking(map * m, point a, point b, bool include_terrain, bool include_NPCs, bool include_endpoint)
{
    bresenham(a,b);

    int min_i = 1;

    if (include_endpoint)
        min_i = 0;

    if (isAt(a,b))
        return false;

    if (getLineSize() == 0)
        return true;

    if (min_i >= getLineSize())
        return true;

    for (int i = min_i; i < getLineSize(); ++i)
    {
        if (include_terrain && isCellBlocking(m->getTerrain(points[i])))
            return true;
        if (include_NPCs && isCellBlocking(m->getNPC(points[i])))
            return true;
    }

    return false;
}

point integer_line::getLinePoint(int i)
{
    if (i > getLineSize() - 1)
        return point(1,1);

    return points[i];
}
