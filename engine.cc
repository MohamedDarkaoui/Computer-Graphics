#include "easy_image.h"
#include "ini_configuration.h"

#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <list>
#include <tuple>
#include <cmath>
#include <vector>

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

using Lines2D = vector<Line2D>;

//draw lines with real coordinates
img::EasyImage draw2DLines(Lines2D &lines, const int size)
{

    //find X_min, X_max, Y_min, Y_max
    double
    x_min = lines[0].p1.x,
    x_max = lines[0].p1.x,
    y_min = lines[0].p1.y,
    y_max = lines[0].p1.y;

    for (Line2D line : lines)
    {
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

    for (Line2D &line: lines)
    {
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

    for (Line2D &line: lines)
    {
        line.p1.x += dx;
        line.p1.y += dy;
        line.p2.x += dx;
        line.p2.y += dy;

    }

    //draw the lines
    img::EasyImage image((int)image_x,(int)image_y);
    for (Line2D line: lines)
    {
           double m = (line.p2.y - line.p1.y)/(line.p2.x - line.p1.x);
           if ((0 < m && m <= 1.0)||(-1<= m && m < 0.0))
           {
               for (int i = 0; i < line.p2.x - line.p1.x; i++)
               {
                   double xi = line.p1.x + i;
                   double yi = round(line.p1.y + m * i);

                   image((int)xi,(int)yi).red = line.Color.red;
                   image((int)xi,(int)yi).green = line.Color.green;
                   image((int)xi,(int)yi).blue = line.Color.blue;
               }
           }
           else if (m > 1)
           {
               for (int i = 0; i < line.p2.y-line.p1.y; i++)
               {
                   double yi = line.p1.y + i;
                   double xi = round(line.p1.x + i/m);

                   image((int)xi,(int)yi).red = line.Color.red;
                   image((int)xi,(int)yi).green = line.Color.green;
                   image((int)xi,(int)yi).blue = line.Color.blue;
               }
           }
           else if (m<-1)
           {
               for (int i = 0; i < line.p1.y - line.p2.y; i++)
               {
                   double yi = line.p1.y - i;
                   double xi = round(line.p1.x - i/m);

                   image((int)xi,(int)yi).red = line.Color.red;
                   image((int)xi,(int)yi).green = line.Color.green;
                   image((int)xi,(int)yi).blue = line.Color.blue;
               }
           }
    }
    return image;
}

img::EasyImage generate_image(const ini::Configuration &configuration)
{

    if (configuration["General"]["type"].as_string_or_die() == "2DLSystem") {
        Lines2D lines;
        Line2D l1 = {{0,   -70},
                     {60,   60},
                     {255, 0, 0}};
        Line2D l2 = {{60, 60},
                     {120, 0},
                     {0, 255, 0}};

        lines.push_back(l1);
        lines.push_back(l2);
        return draw2DLines(lines, 1000);
    }
    return img::EasyImage();
}


int main(int argc, char const* argv[])
{
        int retVal = 0;
        try
        {
                for(int i = 1; i < argc; ++i)
                {
                        ini::Configuration conf;
                        try
                        {
                                std::ifstream fin(argv[i]);
                                fin >> conf;
                                fin.close();
                        }
                        catch(ini::ParseException& ex)
                        {
                                std::cerr << "Error parsing file: " << argv[i] << ": " << ex.what() << std::endl;
                                retVal = 1;
                                continue;
                        }

                        img::EasyImage image = generate_image(conf);
                        if(image.get_height() > 0 && image.get_width() > 0)
                        {
                                std::string fileName(argv[i]);
                                std::string::size_type pos = fileName.rfind('.');
                                if(pos == std::string::npos)
                                {
                                        //filename does not contain a '.' --> append a '.bmp' suffix
                                        fileName += ".bmp";
                                }
                                else
                                {
                                        fileName = fileName.substr(0,pos) + ".bmp";
                                }
                                try
                                {
                                        std::ofstream f_out(fileName.c_str(),std::ios::trunc | std::ios::out | std::ios::binary);
                                        f_out << image;

                                }
                                catch(std::exception& ex)
                                {
                                        std::cerr << "Failed to write image to file: " << ex.what() << std::endl;
                                        retVal = 1;
                                }
                        }
                        else
                        {
                                std::cout << "Could not generate image for " << argv[i] << std::endl;
                        }
                }
        }
        catch(const std::bad_alloc &exception)
        {
    		//When you run out of memory this exception is thrown. When this happens the return value of the program MUST be '100'.
    		//Basically this return value tells our automated test scripts to run your engine on a pc with more memory.
    		//(Unless of course you are already consuming the maximum allowed amount of memory)
    		//If your engine does NOT adhere to this requirement you risk losing points because then our scripts will
		//mark the test as failed while in reality it just needed a bit more memory
                std::cerr << "Error: insufficient memory" << std::endl;
                retVal = 100;
        }
        return retVal;
}
