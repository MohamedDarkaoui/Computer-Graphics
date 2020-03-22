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
#include <fstream>
#include "parser/l_parser.h"

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
img::EasyImage draw2DLines(Lines2D &lines, const int size){

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

    //draw the lines
    img::EasyImage image((int)image_x,(int)image_y);
    for (Line2D line: lines){
        double m = (line.p2.y - line.p1.y)/(line.p2.x - line.p1.x);
        if ((0 < m && m <= 1.0)||(-1<= m && m < 0.0)){
            for (int i = 0; i < line.p2.x - line.p1.x; i++){
                double xi = line.p1.x + i;
                double yi = round(line.p1.y + m * i);

                image((int)xi,(int)yi).red = line.Color.red;
                image((int)xi,(int)yi).green = line.Color.green;
                image((int)xi,(int)yi).blue = line.Color.blue;
            }
        }
        else if (m > 1){
            for (int i = 0; i < line.p2.y-line.p1.y; i++){
                double yi = line.p1.y + i;
                double xi = round(line.p1.x + i/m);

                image((int)xi,(int)yi).red = line.Color.red;
                image((int)xi,(int)yi).green = line.Color.green;
                image((int)xi,(int)yi).blue = line.Color.blue;
            }
        }
        else if (m<-1){
            for (int i = 0; i < line.p1.y - line.p2.y; i++){
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

//parse
LParser::LSystem2D l_system(const string& filename){
    LParser::LSystem2D l_syst;
    std::ifstream input_stream(filename);
    input_stream >> l_syst;
    input_stream.close();
    return l_syst;
}

//returns the L-system string
string getString(const LParser::LSystem2D &l_system, unsigned int n, string S){

    string returnString;
    for (auto c: S){
        if (c != '+' && c != '-')
            returnString += l_system.get_replacement(c);
        else
            returnString += c;
    }

    if (n == 0)
        return returnString;
    else if(n > 0)
        getString(l_system,n-1,returnString);

    return "";
}


//returns a vector of lines
Lines2D drawLSystem(const LParser::LSystem2D &l_system, tuple<double,double,double> color){
    Lines2D lines;
    point current_position = {0,0};
    double pi = 3.14159265359;

    string S = getString(l_system,l_system.get_nr_iterations(),l_system.get_initiator());

    double starting_angle = l_system.get_starting_angle();
    starting_angle = starting_angle*(pi/180);

    double angle = l_system.get_angle();
    angle = angle * (pi/180);

    while (angle > 2*pi){
        angle = angle - 2*pi;
    }

    while (starting_angle > 2*pi){
        starting_angle = starting_angle-2*pi;
    }

    for (auto c:S){
        if (l_system.draw(c)){
            point p1 = current_position;
            point p2 = {current_position.x + cos(starting_angle), current_position.y + sin(starting_angle)};
            Line2D line = {p1,p2,{get<0>(color),get<1>(color),get<2>(color)}};
            lines.push_back(line);
            current_position = p2;
        }
        else if (c != '+' && c != '-' && !l_system.draw(c)){
            current_position.x = current_position.x + cos(starting_angle);
            current_position.y = current_position.y + sin(starting_angle);
        }
        else if (c == '+'){
            starting_angle += angle;
        }
        else if (c == '-'){
            starting_angle -= angle;
        }
    }
    return lines;
}

