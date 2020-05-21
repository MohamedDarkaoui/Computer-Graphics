#include "ZBuffering_lines.h"
#include <limits>

using namespace std;

#include "../easy_image/easy_image.h"
#include <fstream>
#include <stdexcept>
#include <string>
#include <list>
#include <tuple>
#include <cmath>
#include <cctype>
#include "../parser/l_parser.h"
#include "../Surface.h"
#include <limits>




void imageSize(Lines2D &lines, int size, double &x_min, double &x_max, double &y_min, double &y_max, double &image_x,
                double &image_y, double &x_range, double &y_range){
    //find X_min, X_max, Y_min, Y_max

        x_min = lines[0].p1.x;
        x_max = lines[0].p1.x;
        y_min = lines[0].p1.y;
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
        x_range = x_max - x_min,
        y_range = y_max - y_min;


    //calculate the size of the image

        image_x = (size * x_range)/max(x_range,y_range);
        image_y = (size * y_range)/(max(x_range,y_range));


}


//draw lines with real coordinates
img::EasyImage draw_Zbuf_Lines(Lines2D &lines, const int size, tuple<double,double,double> &background_color, ZBuffer &buffer){

    double x_min, x_max, y_min, y_max, image_x, image_y, x_range, y_range;

    imageSize(lines,size,x_min,x_max,y_min,y_max,image_x,image_y,x_range,y_range);

    //scale
    double d = (0.95 * image_x)/x_range;

    for (Line2D &line: lines){
        line.p1.x *= d;
        line.p1.y *= d;
        line.p2.x *= d;
        line.p2.y *= d;
    }

    //move the lines
    double
            DC_x = d * (x_min + x_max)/2,
            DC_y = d * (y_min + y_max)/2,
            dx = (image_x/2)  - DC_x,
            dy = (image_y/2) - DC_y;

    for (Line2D &line: lines){
        line.p1.x += dx;
        line.p1.y += dy;
        line.p2.x += dx;
        line.p2.y += dy;
    }


    img::EasyImage image((int)image_x,(int)image_y);

    //paint the background
    for (int i = 0; i < (int)image_x; i++){
        for (int j = 0; j < (int)image_y; j++){
            image(i,j).red = get<0>(background_color);
            image(i,j).green = get<1>(background_color);
            image(i,j).blue = get<2>(background_color);
        }
    }


    //draw the lines
    for (Line2D line: lines){

        img::Color color;
        color.blue = line.Color.blue;
        color.red = line.Color.red;
        color.green = line.Color.green;
        image.draw_line((unsigned int)line.p1.x,(unsigned int)line.p1.y,(unsigned int)line.p2.x,(unsigned int)line.p2.y,color);

//        if (line.p1.y == line.p2.y){
//            if (line.p1.x < line.p2.x){
//                for (int i = (int)line.p2.x - 1; i >= (int)line.p1.x; i--){
//                    int j = i - (int)line.p2.x;
//                    int a = (int)line.p2.x - (int)line.p1.x;
//
//                    double _z = buffer[i][(int)line.p2.y];
//                    if (line.z1 != 0 && line.z2 != 0)
//                        _z = ((double)j / a)/line.z1 + (1 - (double)j/a)/line.z2;
//
//                    if (buffer[i][(int)line.p2.y] > _z){
//                        buffer[i][(int)line.p2.y] = _z;
//
//                        image(i,(int)line.p2.y).red = line.Color.red;
//                        image(i,(int)line.p2.y).green = line.Color.green;
//                        image(i,(int)line.p2.y).blue = line.Color.blue;
//
//                    }
//                }
//            }
//            else {
//                for (int i = (int)line.p1.x -1 ; i >= line.p2.x; i--){
//                    int j = i - (int)line.p1.x;
//                    int a = (int)line.p1.x - (int)line.p2.x;
//
//                    double _z = buffer[i][(int)line.p2.y];
//                    if (line.z1 != 0 && line.z2 != 0)
//                        _z = ((double)j / a)/line.z2 + (1 - (double)j/a)/line.z1;
//
//                    if (buffer[i][(int)line.p2.y] > _z) {
//                        buffer[i][(int) line.p2.y] = _z;
//
//                        image(i, (int) line.p2.y).red = line.Color.red;
//                        image(i, (int) line.p2.y).green = line.Color.green;
//                        image(i, (int) line.p2.y).blue = line.Color.blue;
//                    }
//                }
//            }
//        }
//
//        else if (line.p1.x == line.p2.x){
//            if (line.p1.y < line.p2.y){
//                for (int i = (int)line.p2.y -1 ; i >= line.p1.y; i--){
//                    int j = i - (int)line.p2.y;
//                    int a = (int)line.p2.y - (int)line.p1.y;
//                    double _z = buffer[(int) line.p2.x][i];
//                    if (line.z1 != 0 && line.z2 != 0)
//                        _z = ((double)j / a)/line.z1 + (1 - (double)j/a)/line.z2;
//
//                    if (buffer[(int) line.p2.x][i] > _z) {
//                        buffer[(int) line.p2.x][i] = _z;
//
//                        image((int) line.p2.x, i).red = line.Color.red;
//                        image((int) line.p2.x, i).green = line.Color.green;
//                        image((int) line.p2.x, i).blue = line.Color.blue;
//
//                    }
//                }
//            }
//            else {
//
//                for (int i = (int)line.p1.y -1; i >= line.p2.y; i--){
//
//                    int j = i - (int)line.p1.y;
//                    int a = (int)line.p1.y - (int)line.p2.y;
//                    double _z = buffer[(int) line.p2.x][i];
//                    if (line.z2 != 0 && line.z1 != 0)
//                        _z = ((double)j / a)/line.z1 + (1 - (double)j/a)/line.z2;
//
//                    if (buffer[(int) line.p2.x][i] > _z) {
//                        buffer[(int) line.p2.x][i] = _z;
//
//                        image((int) line.p2.x, i).red = line.Color.red;
//                        image((int) line.p2.x, i).green = line.Color.green;
//                        image((int) line.p2.x, i).blue = line.Color.blue;
//                    }
//                }
//            }
//        }
//
//        point2D A{};
//        point2D B{};
//        double z1,z2;
//
//
//        if (line.p1.x <= line.p2.x){
//            A = line.p1;
//            B = line.p2;
//            z1 = line.z1;
//            z2 = line.z2;
//        }
//        else {
//            B = line.p1;
//            A = line.p2;
//            z1 = line.z2;
//            z2 = line.z1;
//        }
//
//        double m = 0;
//        if (B.x != A.x) {
//            m = (B.y - A.y)/(B.x - A.x);
//        }
//        if ((0 < m && m <=1) || (-1 <= m && m <0)){
//            for (int i = (int)B.x -  (int)A.x; i >=0 ; i--){
//                double xi = A.x + i;
//                double yi = round(A.y + m*i);
//
//                int a = (int)B.x - (int)A.x;
////                int j = (int)B.x - (int)A.x - i - 1;
//                double _z = ((double)i / a)/line.z1 + (1 - (double)i/a)/line.z2;
//
//                if (buffer[(int)xi][(int)yi] > _z) {
//                    buffer[(int)xi][(int)yi] = _z;
//
//                    image((int) xi, (int) yi).red = line.Color.red;
//                    image((int) xi, (int) yi).green = line.Color.green;
//                    image((int) xi, (int) yi).blue = line.Color.blue;
//                }
//            }
//        }
//
//        else if (m > 1){
//            for (int i = (int)B.y - (int)A.y; i >= 0; i--){
//                double xi = round(A.x + i/m);
//                double yi = A.y + i;
//
//                int a = (int)B.y - (int)A.y;
////                int j = (int)B.y - (int)A.y - i - 1;
//                double _z = ((double)i / a)/line.z1 + (1 - (double)i/a)/line.z2;
//
//                if (buffer[(int)xi][(int)yi] > _z) {
//                    buffer[(int) xi][(int) yi] = _z;
//
//                    image((int) xi, (int) yi).red = line.Color.red;
//                    image((int) xi, (int) yi).green = line.Color.green;
//                    image((int) xi, (int) yi).blue = line.Color.blue;
//                }
//            }
//        }
//        else if (m < -1){
//            for (int i =  (int)A.y - (int)B.y; i >= 0; i--){
//                double xi = round(A.x - i/m);
//                double yi = A.y - i;
//
//                int a = (int)A.y - (int)B.y;
////                int j = (int)A.y - (int)B.y - i - 1;
//                double _z = ((double)i/a)/line.z1 + (1 - (double)i/a)/line.z2;
//
//                if (buffer[(int)xi][(int)yi] > _z) {
//                    buffer[(int) xi][(int) yi] = _z;
//
//                    image((int) xi, (int) yi).red = line.Color.red;
//                    image((int) xi, (int) yi).green = line.Color.green;
//                    image((int) xi, (int) yi).blue = line.Color.blue;
//                }
//            }
//        }
    }


    return image;
}
