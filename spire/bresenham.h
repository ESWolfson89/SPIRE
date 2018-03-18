#ifndef BRESENHAM_H_
#define BRESENHAM_H_

#include "map.h"

class integer_line
{
    public:
        integer_line();
        int getLineSize();
        void clearPoints();
        void bresenham(point, point);
        void checkAllBoundaries();
        bool isBlocking(map *, point, point, bool, bool, bool);
        bool validLinePoint(point);
        point getLinePoint(int);

    private:
        VECTOR <point> points;
};

#endif
