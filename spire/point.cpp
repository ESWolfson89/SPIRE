// point class implementation:

#include "point.h"

// default const.
point::point()
{
    x_val = y_val = 0;
}

point::point(int xx, int yy)
{
    x_val = xx;
    y_val = yy;
}

// set...
void point::set(int xx, int yy)
{
    x_val = xx;
    y_val = yy;
}

// set y only
void point::sety(int yy)
{
    y_val = yy;
}

// set x only
void point::setx(int xx)
{
    x_val = xx;
}

// get y value of point.
int point::y()
{
    return y_val;
}

// get x value of point.
int point::x()
{
    return x_val;
}

// assignment overload
point point::operator=(point p)
{
    x_val = p.x();
    y_val = p.y();
    return *this;
}

bool inRange(point p, int x1, int y1, int x2, int y2)
{
    return (p.x() >= x1 && p.y() >= y1 && p.x() <= x2 && p.y() <= y2);
}

bool isAt(point a, point b)
{
    return (a.x() == b.x() && a.y() == b.y());
}

bool isAdj(point a, point b)
{
    return pointDist(a,b) == 1;
}

int pointDist(point a, point b)
{
    point dab = point(std::abs(a.y() - b.y()), std::abs(a.x()-b.x()));
    return (dab.y() > dab.x()) ? dab.y() : dab.x();
}

point addPoints(point a, point b)
{
    return point(a.x() + b.x(),a.y() + b.y());
}
