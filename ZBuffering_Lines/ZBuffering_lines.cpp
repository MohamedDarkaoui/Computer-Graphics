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
#include <cassert>

void draw_Zbuff_line(img::EasyImage &image, unsigned int x0, unsigned int y0, unsigned int x1, unsigned int y1,
        const img::Color& color, double z0, double z1, ZBuffer& buffer)
{
    assert(x0 < image.get_width() && y0 < image.get_height());
    assert(x1 < image.get_width() && y1 < image.get_height());
    if (x0 == x1)
    {
        //special case for x0 == x1
        int k = std::max(y0, y1);
        int l = std::min(y0, y1);
        for ( int i = k - l; i >= 0; i--)
        {
            unsigned int a = std::max(y0, y1) - std::min(y0, y1);
            double _z = ((double)i / a)/z1 + (1 - (double)i/a)/z0;

            unsigned int j = i + std::min(y0, y1);
            if (buffer[x0][j] > _z){
                buffer[x0][j] = _z;
                image(x0, j) = color;
            }
        }
    }
    else if (y0 == y1)
    {
        //special case for y0 == y1
        int k = std::max(x0, x1);
        int l = std::min(x0, x1);
        for (int i = k - l; i >= 0 ; i--)
        {
            unsigned int a = std::max(x0, x1) - std::min(x0, x1);
            double _z = ((double)i / a)/z1 + (1 - (double)i/a)/z0;

            unsigned int j = i + std::min(x0, x1);
            if (buffer[j][y0] > _z) {
                buffer[j][y0] = _z;
                image(j, y0) = color;
            }
        }
    }
    else
    {
        if (x0 > x1)
        {
            //flip points if x1>x0: we want x0 to have the lowest value
            std::swap(x0, x1);
            std::swap(y0, y1);
            std::swap(z0, z1);
        }
        double m = ((double) y1 - (double) y0) / ((double) x1 - (double) x0);
        if (-1.0 <= m && m <= 1.0)
        {
            for (int i = (int)(x1 - x0) ; i >= 0; i--)
            {
                unsigned int a = x1 - x0;
                double _z = ((double)i / a)/z1 + (1 - (double)i/a)/z0;
                if (buffer[x0+i][(unsigned int) round(y0 + m * i)] > _z) {
                    buffer[x0+i][(unsigned int) round(y0 + m * i)] = _z;

                    image(x0 + i, (unsigned int) round(y0 + m * i)) = color;
                }
            }
        }
        else if (m > 1.0)
        {
            for (int i = (int)(y1 - y0); i >= 0; i--)
            {
                unsigned int a = y1 - y0;
                double _z = ((double)i / a)/z1 + (1 - (double)i/a)/z0;

                if (buffer[(unsigned int) round(x0 + (i / m))][y0 + i] > _z) {
                    buffer[(unsigned int) round(x0 + (i / m))][y0 + i] = _z;
                    image((unsigned int) round(x0 + (i / m)), y0 + i) = color;
                }
            }
        }
        else if (m < -1.0)
        {
            for (int i = (int)(y0 - y1); i >= 0; i--)
            {
                unsigned int a = y0 - y1;
                double _z = ((double)i / a)/z1 + (1 - (double)i/a)/z0;

                if (buffer[(unsigned int) round(x0 - (i / m))][y0 - i] > _z) {
                    buffer[(unsigned int) round(x0 - (i / m))][y0 - i] = _z;
                    image((unsigned int) round(x0 - (i / m)), y0 - i) = color;
                }
            }
        }
    }
}


//draw lines with real coordinates
img::EasyImage draw_Zbuf_Lines(Lines2D &lines, const int size, tuple<double,double,double> &background_color){

    double x_min, x_max, y_min, y_max, image_x, image_y, x_range, y_range;

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

    ZBuffer zBuffer = ZBuffer((int)image_x,(int)image_y);
    //draw the lines
    for (Line2D line: lines) {

        img::Color color;
        color.blue = line.Color.blue;
        color.red = line.Color.red;
        color.green = line.Color.green;
        draw_Zbuff_line(image, (unsigned int) line.p1.x, (unsigned int) line.p1.y, (unsigned int) line.p2.x,
                        (unsigned int) line.p2.y, color, line.z1, line.z2, zBuffer);

    }
    return image;
}
