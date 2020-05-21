#include <fstream>
#include <stdexcept>
#include <string>
#include <list>
#include <tuple>
#include <cmath>
#include <cctype>
#include "../parser/l_parser.h"
#include "../Surface.h"



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
Lines2D draw2DLSystem(const LParser::LSystem2D &l_system, tuple<double,double,double> color){
    Lines2D lines;
    point2D current_position = {0,0};
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


    vector <pair<point2D,double>> stack;
    unsigned int iterations = l_system.get_nr_iterations();
    const string& initiator = l_system.get_initiator();
    string S;

    getString(l_system,iterations,initiator,S);

       for (auto c:S){
        if (isalnum(c)){
            if (l_system.draw(c)) {
                point2D p1 = current_position;
                point2D p2 = {current_position.x + cos(starting_angle), current_position.y + sin(starting_angle)};
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

