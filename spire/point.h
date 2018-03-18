// Header file for point class

#ifndef POINT_H_
#define POINT_H_

#include <cmath>

class point
{
    public:
        // default const.       
        point();
        // copy const.
        point(int, int);
        // set...
        void set(int, int);
        // set y only
        void sety(int);
        // set x only
        void setx(int);
        // get y value of point.
        int y();
        // get x value of point.
        int x();
        // assignment overload
        point operator=(point);
    private:
        // member variables for x and y coord.
        int x_val,y_val;
};

bool inRange(point, int, int, int, int);

bool isAt(point,point);

bool isAdj(point, point);

int pointDist(point, point);

point addPoints(point a, point b);

#endif
