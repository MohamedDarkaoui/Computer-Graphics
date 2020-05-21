#include "../easy_image/easy_image.h"
#include <tuple>
#include <cmath>
#include "../Surface.h"



Figure createTetrahedron(){
    Figure figure;

    //points
    Vector3D p1 = Vector3D::point(1,-1,-1);
    Vector3D p2 = Vector3D::point(-1,1,-1);
    Vector3D p3 = Vector3D::point(1,1,1);
    Vector3D p4 = Vector3D::point(-1,-1,1);

    figure.points = {p1,p2,p3,p4};

    //faces
    Face f1, f2, f3, f4;
    f1.point_indexes = {0,1,2};
    f2.point_indexes = {1,3,2};
    f3.point_indexes = {0,3,1};
    f4.point_indexes = {0,2,3};

    figure.faces = {f1,f2,f3,f4};

    return figure;
}

Figure createCube(){

    Figure figure;
    Vector3D p1 = Vector3D::point(1,-1,-1);
    Vector3D p2 = Vector3D::point(-1,1,-1);
    Vector3D p3 = Vector3D::point(1,1,1);
    Vector3D p4 = Vector3D::point(-1,-1,1);
    Vector3D p5 = Vector3D::point(1,1,-1);
    Vector3D p6 = Vector3D::point(-1,-1,-1);
    Vector3D p7 = Vector3D::point(1,-1,1);
    Vector3D p8 = Vector3D::point(-1,1,1);

    figure.points = {p1,p2,p3,p4,p5,p6,p7,p8};

    Face f1, f2, f3, f4, f5, f6;

    f1.point_indexes = {0,4,2,6};
    f2.point_indexes = {4,1,7,2};
    f3.point_indexes = {1,5,3,7};
    f4.point_indexes = {5,0,6,3};
    f5.point_indexes = {6,2,7,3};
    f6.point_indexes = {0,5,1,4};

    figure.faces = {f1,f2,f3,f4,f5,f6};

    return figure;
}


Figure createOctahedron(){
    Figure figure;
    Vector3D p1 = Vector3D::point(1,0,0);
    Vector3D p2 = Vector3D::point(0,1,0);
    Vector3D p3 = Vector3D::point(-1,0,0);
    Vector3D p4 = Vector3D::point(0,-1,0);
    Vector3D p5 = Vector3D::point(0,0,-1);
    Vector3D p6 = Vector3D::point(0,0,1);

    figure.points = {p1,p2,p3,p4,p5,p6};

    Face f1,f2,f3,f4,f5,f6,f7,f8;

    f1.point_indexes = {0,1,5};
    f2.point_indexes = {1,2,5};
    f3.point_indexes = {2,3,5};
    f4.point_indexes = {3,0,5};
    f5.point_indexes = {1,0,4};
    f6.point_indexes = {2,1,4};
    f7.point_indexes = {3,2,4};
    f8.point_indexes = {0,3,4};

    figure.faces = {f1,f2,f3,f4,f5,f6,f7,f8};

    return figure;
}


Figure createIcosahedron(){
    double pi = 3.14159265359;
    Figure figure;
    Vector3D p1 = Vector3D::point(0,0,sqrt(5)/2);
    vector<Vector3D> v;
    v.push_back(p1);
    for (int i = 2; i < 7; i++){
        Vector3D p = Vector3D::point(cos((i-2)*2*pi/5),sin((i-2)*2*pi/5),0.5);
        v.push_back(p);
    }
    for (int i = 7; i < 12; i++){
        Vector3D p = Vector3D::point(cos(pi/5 + (i-7)*2*pi/5),sin(pi/5 + (i-7)*2*pi/5),-0.5);
        v.push_back(p);
    }
    Vector3D p12 = Vector3D::point(0,0, -sqrt(5)/2);
    v.push_back(p12);

    figure.points = v;

    Face f1,f2,f3,f4,f5,f6,f7,f8,f9,f10,f11,f12,f13,f14,f15,f16,f17,f18,f19,f20;

    f1.point_indexes = {0,1,2};
    f2.point_indexes = {0,2,3};
    f3.point_indexes = {0,3,4};
    f4.point_indexes = {0,4,5};
    f5.point_indexes = {0,5,1};
    f6.point_indexes = {1,6,2};
    f7.point_indexes = {2,6,7};
    f8.point_indexes = {2,7,3};
    f9.point_indexes = {3,7,8};
    f10.point_indexes = {3,8,4};
    f11.point_indexes = {4,8,9};
    f12.point_indexes = {4,9,5};
    f13.point_indexes = {5,9,10};
    f14.point_indexes = {5,10,1};
    f15.point_indexes = {1,10,6};
    f16.point_indexes = {11,7,6};
    f17.point_indexes = {11,8,7};
    f18.point_indexes = {11,9,8};
    f19.point_indexes = {11,10,9};
    f20.point_indexes = {11,6,10};

    figure.faces = {f1,f2,f3,f4,f5,f6,f7,f8,f9,f10,f11,f12,f13,f14,f15,f16,f17,f18,f19,f20};

    return figure;
}


Figure createDodecahedron(){
    Figure icosahedron = createIcosahedron();
    Figure figure;
    figure.points = {};
    for (unsigned int i = 0; i < icosahedron.faces.size(); i++){
        double x,y,z;
        x = icosahedron.points[icosahedron.faces[i].point_indexes[0]].x;
        x += icosahedron.points[icosahedron.faces[i].point_indexes[1]].x;
        x += icosahedron.points[icosahedron.faces[i].point_indexes[2]].x;
        x /= 3;

        y = icosahedron.points[icosahedron.faces[i].point_indexes[0]].y;
        y += icosahedron.points[icosahedron.faces[i].point_indexes[1]].y;
        y += icosahedron.points[icosahedron.faces[i].point_indexes[2]].y;
        y /= 3;

        z = icosahedron.points[icosahedron.faces[i].point_indexes[0]].z;
        z += icosahedron.points[icosahedron.faces[i].point_indexes[1]].z;
        z += icosahedron.points[icosahedron.faces[i].point_indexes[2]].z;
        z /= 3;

        Vector3D p = Vector3D::point(x,y,z);

        figure.points.push_back(p);
    }

    Face f1,f2,f3,f4,f5,f6,f7,f8,f9,f10,f11,f12;

    f1.point_indexes = {0,1,2,3,4};
    f2.point_indexes = {0,5,6,7,1};
    f3.point_indexes = {1,7,8,9,2};
    f4.point_indexes = {2,9,10,11,3};
    f5.point_indexes = {3,11,12,13,4};
    f6.point_indexes = {4,13,14,5,0};
    f7.point_indexes = {19,18,17,16,15};
    f8.point_indexes = {19,14,13,12,18};
    f9.point_indexes = {18,12,11,10,17};
    f10.point_indexes = {17,10,9,8,16};
    f11.point_indexes = {16,8,7,6,15};
    f12.point_indexes = {15,6,5,14,19};

    figure.faces = {f1,f2,f3,f4,f5,f6,f7,f8,f9,f10,f11,f12};


    return figure;
}

//splits all triangles of the icosahedron into n triangles
Figure splitTriangles(Figure &icosahedron, int n){
    Figure figure;
    figure.points = {};
    figure.faces = {};

    if (n == 0)
        return  icosahedron;


    for (unsigned int i = 0; i < icosahedron.faces.size(); i++){

        Face face1, face2, face3,face4;

        double xd = (icosahedron.points[icosahedron.faces[i].point_indexes[0]].x + icosahedron.points[icosahedron.faces[i].point_indexes[1]].x)/2;
        double yd = (icosahedron.points[icosahedron.faces[i].point_indexes[0]].y + icosahedron.points[icosahedron.faces[i].point_indexes[1]].y)/2;
        double zd = (icosahedron.points[icosahedron.faces[i].point_indexes[0]].z + icosahedron.points[icosahedron.faces[i].point_indexes[1]].z)/2;
        Vector3D D = Vector3D::point(xd,yd,zd);

        double xe = (icosahedron.points[icosahedron.faces[i].point_indexes[0]].x + icosahedron.points[icosahedron.faces[i].point_indexes[2]].x)/2;
        double ye = (icosahedron.points[icosahedron.faces[i].point_indexes[0]].y + icosahedron.points[icosahedron.faces[i].point_indexes[2]].y)/2;
        double ze = (icosahedron.points[icosahedron.faces[i].point_indexes[0]].z + icosahedron.points[icosahedron.faces[i].point_indexes[2]].z)/2;
        Vector3D E = Vector3D::point(xe,ye,ze);

        double xf = (icosahedron.points[icosahedron.faces[i].point_indexes[1]].x + icosahedron.points[icosahedron.faces[i].point_indexes[2]].x)/2;
        double yf = (icosahedron.points[icosahedron.faces[i].point_indexes[1]].y + icosahedron.points[icosahedron.faces[i].point_indexes[2]].y)/2;
        double zf = (icosahedron.points[icosahedron.faces[i].point_indexes[1]].z + icosahedron.points[icosahedron.faces[i].point_indexes[2]].z)/2;
        Vector3D F = Vector3D::point(xf,yf,zf);

        Vector3D A = icosahedron.points[icosahedron.faces[i].point_indexes[0]];
        Vector3D B = icosahedron.points[icosahedron.faces[i].point_indexes[1]];
        Vector3D C = icosahedron.points[icosahedron.faces[i].point_indexes[2]];

        vector<Vector3D> temp = {A,B,C,D,E,F};

        figure.points.push_back(A);
        figure.points.push_back(B);
        figure.points.push_back(C);
        figure.points.push_back(D);
        figure.points.push_back(E);
        figure.points.push_back(F);

        int j = (int)i;
        face1.point_indexes = {j*6, j*6+3, j*6+4};
        face2.point_indexes = {j*6+1, j*6+5, j*6+3};
        face3.point_indexes = {j*6+2, j*6+4,j*6+5};
        face4.point_indexes = {j*6+3,j*6+5,j*6+4};

        figure.faces.push_back(face1);
        figure.faces.push_back(face2);
        figure.faces.push_back(face3);
        figure.faces.push_back(face4);
    }

    return splitTriangles(figure,n-1);
}

Figure createSphere(const int n){
    Figure icosahedron = createIcosahedron();
    Figure figure = splitTriangles(icosahedron,n);

    for (auto& point : figure.points){
        double r = sqrt(pow(point.x,2) + pow(point.y,2) + pow(point.z,2));
        point.x = point.x/r;
        point.y = point.y/r;
        point.z = point.z/r;
    }
    return figure;
}

Figure createCone(const int n, const double h){
    Figure figure;
    figure.points = {};
    figure.faces = {};

    double pi = 3.14159265359;
    Vector3D Pn = Vector3D::point(0,0,h);
    figure.points.push_back(Pn);
    for (int i = 0; i < n; i++){
        Vector3D Pi = Vector3D::point(cos(2*i*pi/n), sin(2*i*pi/n),0);
        figure.points.push_back(Pi);
    }

    for (int i = 1; i < n+1; i++){
        Face face;

        face.point_indexes = {i,(i+1)%n,0};
        figure.faces.push_back(face);
    }

    Face face1;
    for(int i = n; i >= 0 ; i--){
        face1.point_indexes.push_back(i);
    }

    figure.faces.push_back(face1);

    return figure;
}

Figure createCylinder(const int n, const double h){
    Figure figure;
    figure.points = {};
    figure.faces = {};

    double pi = 3.14159265359;

    for (int i = 0; i < n; i++){
        Vector3D Pi = Vector3D::point(cos(2*i*pi/n), sin(2*i*pi/n),0);
        figure.points.push_back(Pi);
    }

    for (int i = 0; i < n; i++){
        Vector3D Pi = Vector3D::point(cos(2*i*pi/n), sin(2*i*pi/n),h);
        figure.points.push_back(Pi);
    }

    Face face1;
    face1.point_indexes = {0, n-1,2*n-1,n};
    figure.faces.push_back(face1);

    for (int i = 1; i < n; i++){
        Face face;
        face.point_indexes = {i, i-1, n+i-1, n+i};
        figure.faces.push_back(face);
    }

    return figure;
}

Figure createTorus(const double r, const double R, const int n, const int m){
    Figure figure;
    figure.points = {};
    figure.faces = {};

    vector<vector<Vector3D>> points2;

    double pi = 3.14159265359;

    for (int i = 0; i < n; i++){
        vector<Vector3D> P;
        for (int j = 0; j < m ;j++){
            double v = 2*i*pi/n;
            double u = 2*j*pi/m;
            Vector3D p = Vector3D::point((R + r*cos(v))*cos(u), (R + r*cos(v))*sin(u), r*sin(v));
            P.push_back(p);
            figure.points.push_back(p);
        }
        points2.push_back(P);
    }


    for (unsigned int i = 0; i < figure.points.size() - m ; i++){
        Face face;
        int j = (int) i;
        if ((j + 1) % m != 0) {
            face.point_indexes = {j, j + 1, j + 1 + m, j + m};
            figure.faces.push_back(face);
        }
        else {
            face.point_indexes = {j, j + 1 - m, j + 1, j + m};
            figure.faces.push_back(face);
        }

    }

    for (int i = 0; i < m -1; i++){
        Face face;
        int j = (int) figure.points.size() - m + i;
        face.point_indexes = {j,j+1,i+1,i};
        figure.faces.push_back(face);
    }
    
    return figure;
}