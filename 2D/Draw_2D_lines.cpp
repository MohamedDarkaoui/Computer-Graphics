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

//draw lines with real coordinates
img::EasyImage draw2DLines(Lines2D &lines, const int size, tuple<double,double,double> &background_color){

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
        color.red = line.Color.red;
        color.blue = line.Color.blue;
        color.green = line.Color.green;
        image.draw_line((unsigned int)line.p1.x,(unsigned int)line.p1.y,(unsigned int)line.p2.x,
                (unsigned int)line.p2.y,color);

    }
    return image;
}
