#ifndef ENGINE_SURFACE_H
#define ENGINE_SURFACE_H

#include <iostream>
#include <vector>
#include "vector3d.h"



using namespace std;

struct point{
    double x;
    double y;
};

struct color{
    double red;
    double green;
    double blue;
};

struct Line2D{
    point p1;
    point p2;
    color Color;
};

struct Face{
    vector<int> point_indexes;
};

struct Figure {
    vector<Vector3D> points;
    vector<Face> faces;
    color Color;
};

using Lines2D = vector<Line2D>;
typedef  vector<Figure> Figures3D;


#endif //ENGINE_SURFACE_H
