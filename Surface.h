#ifndef ENGINE_SURFACE_H
#define ENGINE_SURFACE_H

#include <iostream>
#include <vector>
#include "vector3d/vector3d.h"
#include <limits>



using namespace std;

struct point2D{
    double x;
    double y;
};

struct point3D{
    double x;
    double y;
    double z;

    point3D(double x, double y, double z) : x(x), y(y), z(z){};
};

struct color{
    double red;
    double green;
    double blue;
};

struct Line2D{
    point2D p1;
    point2D p2;
    color Color;

    double z1 = -1;
    double z2 = -1;
};

struct Face{
    vector<int> point_indexes;
};

struct Figure {
    bool draw_extra_line = true;
    vector<Vector3D> points;
    vector<Face> faces;
    color Color;
};


using Lines2D = vector<Line2D>;
typedef  vector<Figure> Figures3D;

#endif //ENGINE_SURFACE_H
