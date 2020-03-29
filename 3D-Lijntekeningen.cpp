#include "easy_image.h"
#include "ini_configuration.h"
#include <fstream>
#include <stdexcept>
#include <string>
#include <list>
#include <tuple>
#include <cmath>
#include <fstream>
#include <cctype>
#include "parser/l_parser.h"
#include "Surface.h"

Matrix scaleFigure (const double scale){
    Matrix m;
    m (1,1) = scale;
    m (2,2) = scale;
    m (3,3) = scale;
    m (4,4) = 1;

    return m;
}

Matrix rotateX (const double angle){
    Matrix m;
    m (1,1) = 1;
    m (2,2) = cos(angle);
    m (2,3) = sin(angle);
    m (3,2) = -sin(angle);
    m (3,3) = cos(angle);
    m (4,4) = 1;

    return m;
}

Matrix rotateY (const double angle){
    Matrix m;
    m (1,1) = cos(angle);
    m (1,3) = -sin(angle);
    m (2,2) = 1;
    m (3,1) = sin(angle);
    m (3,3) = cos(angle);
    m (4,4) = 1;

    return m;
}

Matrix rotateZ (const double angle){
    Matrix m;
    m (1,1) = cos(angle);
    m (1,2) = sin(angle);
    m (2,1) = -sin(angle);
    m (2,2) = cos(angle);
    m (3,3) = 1;
    m (4,4) = 1;

    return m;
}

Matrix translate (const Vector3D &vector){
    Matrix m;
    m (1,1) = 1;
    m (2,2) = 1;
    m (3,3) = 1;
    m (4,4) = 1;
    m (4,1) = vector.x;
    m (4,2) = vector.y;
    m (4,3) = vector.z;

    return m;
}

void applyTransformation (Figure &f, const Matrix &m){
    for (auto &p : f.points){
        p = p*m;
    }
}

//convert to spherical coordinates
void toPolar(const Vector3D &point, double &theta, double &phi, double &r){
    r = sqrt(pow(point.x,2) + pow(point.y,2) + pow(point.z,2));
    theta = atan2(point.y,point.x);
    phi = acos(point.z/r);

}

Matrix eyepointTrans(const Vector3D &eyepoint){
    double r, theta, phi;
    toPolar(eyepoint,theta,phi,r);

    Matrix m;
    m (1,1) = -sin(theta);
    m (1,2) = -cos(theta)*cos(phi);
    m (1,3) = cos(theta)*sin(phi);
    m (2,1) = cos(theta);
    m (2,2) = -sin(theta)*cos(phi);
    m (2,3) = sin(theta)*sin(phi);
    m (3,2) = sin(phi);
    m (3,3) = cos(phi);
    m (4,3) = -r;
    m (4,4) = 1;

    return m;
}

point2D doProjection(const Vector3D &point, const double d = 1){
    point2D p{};
    p.x = d * point.x/-point.z;
    p.y = d * point.y/-point.z;

    return p;
}


Lines2D doProjection(Figures3D &figures, const vector<vector<tuple<int,int>>> &point_indexes,
                    const Vector3D &eyepoint,const double d = 1){


    Lines2D lines;
    for (unsigned int i = 0; i < figures.size(); i++){
        applyTransformation(figures[i],eyepointTrans(eyepoint));
        for (unsigned int j = 0; j < point_indexes[i].size(); j++){
            point2D p1 = doProjection(figures[i].points[get<0>(point_indexes[i][j])],d);
            point2D p2 = doProjection(figures[i].points[get<1>(point_indexes[i][j])],d);

            Line2D line = {p1,p2,figures[i].Color};
            lines.push_back(line);
        }
    }

    return lines;
}


//Lines2D doProjection(const Figures3D &figures){
//    Lines2D lines;
//
//    for (auto figure: figures){
//        for (auto face: figure.faces){
//            for (unsigned int i = 0; i < face.point_indexes.size()-1; i++){
//                point2D p1 = doProjection(figure.points[face.point_indexes[i]]);
//                point2D p2 = doProjection(figure.points[face.point_indexes[i+1]]);
//                lines.push_back({p1,p2,figure.Color});
//            }
//            lines.push_back({doProjection(figure.points[face.point_indexes[0]]),
//                             doProjection(figure.points[face.point_indexes[face.point_indexes.size()-1]]),
//                             figure.Color});
//        }
//    }
//    return lines;
//}




