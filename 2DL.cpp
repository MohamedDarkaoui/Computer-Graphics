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
        if (line.p1.y == line.p2.y){
            if (line.p1.x < line.p2.x){
                for (int i = (int)line.p1.x; i< line.p2.x; i++){
                    image(i,(int)line.p2.y).red = line.Color.red;
                    image(i,(int)line.p2.y).green = line.Color.green;
                    image(i,(int)line.p2.y).blue = line.Color.blue;
                }
            }
            else{
                for (int i = (int)line.p2.x; i< line.p1.x; i++){
                    image(i,(int)line.p2.y).red = line.Color.red;
                    image(i,(int)line.p2.y).green = line.Color.green;
                    image(i,(int)line.p2.y).blue = line.Color.blue;
                }
            }
        }

        else if (line.p1.x == line.p2.x){
            if (line.p1.y<line.p2.y){
                for (int i = (int)line.p1.y; i< line.p2.y; i++){
                    image((int)line.p2.x,i).red = line.Color.red;
                    image((int)line.p2.x,i).green = line.Color.green;
                    image((int)line.p2.x,i).blue = line.Color.blue;
                }
            }
            else{
                for (int i = (int)line.p2.y; i< line.p1.y; i++){
                    image((int)line.p2.x,i).red = line.Color.red;
                    image((int)line.p2.x,i).green = line.Color.green;
                    image((int)line.p2.x,i).blue = line.Color.blue;
                }
            }
        }

        point A{};
        point B{};

        if (line.p1.x <= line.p2.x){
            A = line.p1;
            B = line.p2;
        }
        else {
            B = line.p1;
            A = line.p2;
        }

        double m = 0;
        if (B.x != A.x) {
            m = (B.y - A.y)/(B.x - A.x);
        }
        if ((0 < m && m <=1)||(-1 <= m && m <0)){
            for (int i = 0; i< B.x -  A.x; i++){
                double xi = A.x + i;
                double yi = round(A.y + m*i);

                image((int)xi, (int)yi).red = line.Color.red;
                image((int)xi, (int)yi).green = line.Color.green;
                image((int)xi, (int)yi).blue = line.Color.blue;
            }
        }

        else if (m > 1){
            for (int i = 0; i < B.y - A.y; i++){
                double xi = round(A.x + i/m);
                double yi = A.y + i;

                image((int)xi, (int)yi).red = line.Color.red;
                image((int)xi, (int)yi).green = line.Color.green;
                image((int)xi, (int)yi).blue = line.Color.blue;
            }
        }
        else if (m < -1){
            for (int i = 0; i <A.y - B.y; i++){
                double xi = round(A.x - i/m);
                double yi = A.y - i;

                image((int)xi, (int)yi).red = line.Color.red;
                image((int)xi, (int)yi).green = line.Color.green;
                image((int)xi, (int)yi).blue = line.Color.blue;

            }
        }
    }
    return image;
}

//parse
LParser::LSystem2D l_system(const string& filename){
    LParser::LSystem2D l_syst;
    ifstream input_stream(filename);
    input_stream >> l_syst;
    input_stream.close();
    return l_syst;
}

//returns the L-system string
void getString(const LParser::LSystem2D &l_system, unsigned int n, const string &S, string &k){

    string returnString;
    for (auto c: S){
        if (isalnum(c)) {
            returnString += l_system.get_replacement(c);
        }
        else if (c == '+' || c == '-' || c == '(' || c == ')')
            returnString += c;
    }

    if (n == 1 || n == 0) {
        k = returnString;
        return;
    }
    else if(n > 0)
        getString(l_system,n-1,returnString,k);
}


//returns a vector of lines
Lines2D drawLSystem(const LParser::LSystem2D &l_system, tuple<double,double,double> color){
    Lines2D lines;
    point current_position = {0,0};
    double pi = 3.14159265359;


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


    vector <pair<point,double>> stack;
    unsigned int iterations = l_system.get_nr_iterations();
    const string& initiator = l_system.get_initiator();
    string S;

    getString(l_system,iterations,initiator,S);

       for (auto c:S){
        if (isalnum(c)){
            if (l_system.draw(c)) {
                point p1 = current_position;
                point p2 = {current_position.x + cos(starting_angle), current_position.y + sin(starting_angle)};
                Line2D line = {p1, p2, {get<0>(color), get<1>(color), get<2>(color)}};
                lines.push_back(line);
                current_position = p2;
            }
        }
        else if (c != '+' && c != '-' && c != '(' && c != ')'){
            if (!l_system.draw(c)) {
                current_position.x = current_position.x + cos(starting_angle);
                current_position.y = current_position.y + sin(starting_angle);
            }
        }
        else if (c == '+'){
            starting_angle += angle;
        }
        else if (c == '-'){
            starting_angle -= angle;
        }
        else if (c == '('){
            stack.emplace_back(current_position,starting_angle);
        }
        else if (c == ')'){
            current_position = stack.back().first;
            starting_angle = stack.back().second;
            stack.pop_back();
        }
    }
    return lines;
}

