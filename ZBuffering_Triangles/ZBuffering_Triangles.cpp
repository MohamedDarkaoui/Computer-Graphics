#include "../Surface.h"
#include "../ZBuffering_Lines/ZBuffering_lines.h"

//split all figures into triangles
Figures3D triangulate(Figures3D &figures){
    Figures3D returnFigures;

    for (const Figure& figure : figures){
        Figure figure1 = Figure();
        for (const Face& face : figure.faces){
            for (unsigned int i = 1; i < face.point_indexes.size() - 1; i++) {
                Face face1;
                face1.point_indexes = {face.point_indexes[0], face.point_indexes[i], face.point_indexes[i + 1]};
                figure1.faces.push_back(face1);
            }
        }
        figure1.points = figure.points;
        figure1.Color = figure.Color;
        figure1.draw_extra_line = figure.draw_extra_line;
        returnFigures.push_back(figure1);

    }
    return returnFigures;
}

map<string,double> projection(Lines2D &lines, const int size){
    //find X_min, X_max, Y_min, Y_max
    double
            x_min = lines[0].p1.x,
            x_max = lines[0].p1.x,
            y_min = lines[0].p1.y,
            y_max = lines[0].p1.y;

    for (Line2D line : lines){
        if (line.p1.x < x_min){x_min = line.p1.x;}
        if (line.p2.x < x_min){x_min = line.p2.x;}
        if (line.p1.x > x_max){x_max = line.p1.x;}
        if (line.p2.x > x_max){x_max = line.p2.x;}
        if (line.p1.y < y_min){y_min = line.p1.y;}
        if (line.p2.y < y_min){y_min = line.p2.y;}
        if (line.p1.y > y_max){y_max = line.p1.y;}
        if (line.p2.y > y_max){y_max = line.p2.y;}
    }

    //find the range
    double
            x_range = x_max - x_min,
            y_range = y_max - y_min;


    //calculate the size of the image
    double
            image_x = (size * x_range)/max(x_range,y_range),
            image_y = (size * y_range)/(max(x_range,y_range));

    //scale
    double d = (0.95 * image_x)/x_range;

    for (Line2D &line: lines){
        line.p1.x *= d;
        line.p1.y *= d;
        line.p2.x *= d;
        line.p2.y *= d;
    }

    double
            DC_x = d * (x_min + x_max)/2,
            DC_y = d * (y_min + y_max)/2,
            dx = (image_x/2)  - DC_x,
            dy = (image_y/2) - DC_y;

    map<string,double> returnMap;

    returnMap["d"] = d;
    returnMap["image_x"] = image_x;
    returnMap["image_y"] = image_y;
    returnMap["dx"] = dx;
    returnMap["dy"] = dy;

    return returnMap;
}

void draw_ZBuf_triangle(ZBuffer &buffer, img::EasyImage &image, Vector3D const& A, Vector3D const &B, Vector3D const &C,
                        double d, double dx, double dy, const img::Color& color){

    /////////// PROJECTION ///////////

    point2D _A = {(d*A.x)/(-A.z) + dx, (d*A.y)/(-A.z) + dy};
    point2D _B = {(d*B.x)/(-B.z) + dx, (d*B.y)/(-B.z) + dy};
    point2D _C = {(d*C.x)/(-C.z) + dx, (d*C.y)/(-C.z) + dy};

    /////////// TRIANGLE PIXELS ///////////

    vector<point2D> p = {_A,_B,_C};
    double y_min = _A.y;
    double y_max = _A.y;
    for (point2D point : p){
        if (point.y < y_min){
            y_min = point.y;
        }
    }

    for (point2D point : p){
        if (point.y > y_max){
            y_max = point.y;
        }
    }

    vector<pair<point2D,point2D>> line_segments;
    line_segments.emplace_back(_A,_B);
    line_segments.emplace_back(_A,_C);
    line_segments.emplace_back(_B,_C);

    for (int y = (int)round(y_min + 0.5); y <= round(y_max - 0.5); y++){

        double xl_AB, xl_AC, xl_BC, xr_AB, xr_AC, xr_BC;
        xl_AB = numeric_limits<double>::infinity();
        xl_AC = numeric_limits<double>::infinity();
        xl_BC = numeric_limits<double>::infinity();

        xr_AB = -numeric_limits<double>::infinity();
        xr_AC = -numeric_limits<double>::infinity();
        xr_BC = -numeric_limits<double>::infinity();

        for (auto line_segment : line_segments){
            if ((y - line_segment.first.y) * (y - line_segment.second.y) <= 0 &&
                line_segment.first.y != line_segment.second.y){
                double xp = line_segment.first.x;
                double xq = line_segment.second.x;
                double yq = line_segment.second.y;
                double yp = line_segment.first.y;

                double xi = xq + (xp - xq) * (y - yq)/(yp - yq);

                if (xp == _A.x && yp == _A.y && xq == _B.x && yq == _B.y){
                    xl_AB = xi;
                    xr_AB = xi;
                }
                else if(xp == _A.x && yp == _A.y && xq == _C.x && yq == _C.y){
                    xl_AC = xi;
                    xr_AC = xi;
                }
                else{
                    xl_BC = xi;
                    xr_BC = xi;
                }
            }
        }

        int xl, xr;
        double min, max;

        vector<double> mxl = {xl_AB,xl_AC,xl_BC};
        vector<double> mxr = {xr_AB,xr_AC,xr_BC};

        min = xl_AC;

        for (double _d : mxl){
            if (_d < min){
                min = _d;
            }
        }

        max = xr_AB;

        for (double _d : mxr){
            if (_d > max){
                max = _d;
            }
        }

        xl = (int)round(min + 0.5);
        xr = (int)round(max - 0.5);

        for (int i = xl; i <= xr; i++){
            double xg = (_A.x + _B.x + _C.x)/3;
            double yg = (_A.y + _B.y + _C.y)/3;
            double _zg = 1/(3*A.z) + 1/(3*B.z) + 1/(3*C.z);

            Vector3D u = B - A;
            Vector3D v = C - A;

            double w1 = u.y * v.z - u.z * v.y;
            double w2 = u.z * v.x - u.x * v.z;
            double w3 = u.x * v.y - u.y * v.x;

            double k = w1 * A.x + w2 * A.y + w3 * A.z;

            double dzdx = w1/(-d*k);
            double dzdy = w2/(-d*k);

            double _z = 1.0001 * _zg + (i - xg) * dzdx + (y - yg) * dzdy;

            if (buffer[i][y] >= _z){
                buffer[i][y] = _z;
                image(i,y) = color;
            }
        }
    }
}

img::EasyImage draw_ZBuf_Triangles(Figures3D &figures, int image_x, int image_y,double d, double dx, double dy){

    ZBuffer zBuffer = ZBuffer(image_x,image_y);
    img::EasyImage image(image_x,image_y);

    for (Figure figure : triangulate(figures)){
        img::Color color;
        color.red = figure.Color.red;
        color.green = figure.Color.green;
        color.blue = figure.Color.blue;
        for (Face face : figure.faces){

            Vector3D A = figure.points[face.point_indexes[0]];
            Vector3D B = figure.points[face.point_indexes[1]];
            Vector3D C = figure.points[face.point_indexes[2]];

            draw_ZBuf_triangle(zBuffer,image,A,B,C,d,dx,dy,color);
        }
    }

    return image;
}