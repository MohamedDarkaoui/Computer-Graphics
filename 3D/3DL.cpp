#include <fstream>
#include <stdexcept>
#include <string>
#include <list>
#include <tuple>
#include <cmath>
#include <cctype>
#include "../parser/l_parser.h"
#include "../Surface.h"
#include "../vector3d/vector3d.h"
#include "../3D/3D-Lijntekeningen.cpp"


//parse
LParser::LSystem3D l3D_system(const string& filename){
    LParser::LSystem3D l_syst;
    ifstream input_stream(filename);
    input_stream >> l_syst;
    input_stream.close();
    return l_syst;
}

//returns the L-system string
void get3DLString(const LParser::LSystem3D &l_system, unsigned int n, const string &S, string &k){

    string returnString;
    for (auto c: S){
        if (isalnum(c)) {
            returnString += l_system.get_replacement(c);
        }
        else if (c == '+' || c == '-' || c == '(' || c == ')' || c == '&' || c == '^' || c== '|' || c == '\\' || c == '/')
            returnString += c;
    }

    if (n == 1 || n == 0) {
        k = returnString;
        return;
    }
    else if(n > 0)
        get3DLString(l_system,n-1,returnString,k);
}

//returns a vector of lines
Figure draw3DLSystem(const LParser::LSystem3D &l_system){

    Figure figure;
    figure.points = {};

    double pi = 3.14159265359;

    Vector3D curr_pos = Vector3D::point(0,0,0);

    Vector3D H, L, U;
    H.x = 1;
    L.y = 1;
    U.z = 1;


    double angle = l_system.get_angle();
    angle = angle*(pi/180);


    while (angle > 2*pi){
        angle = angle - 2*pi;
    }

    vector <tuple<Vector3D,Vector3D,Vector3D,Vector3D>> stack;
    unsigned int iterations = l_system.get_nr_iterations();
    const string& initiator = l_system.get_initiator();
    string S;

    get3DLString(l_system,iterations,initiator,S);

    figure.points.push_back(curr_pos);

    for (auto c:S){
        if (isalnum(c)){
            if (l_system.draw(c)) {
                curr_pos = Vector3D::point(curr_pos.x + H.x, curr_pos.y + H.y, curr_pos.z + H.z);
                figure.points.push_back(curr_pos);
            }
        }
        else if (c != '+' && c != '-' && c != '(' && c != ')' && c != '^' && c != '|' && c != '\\' && c != '/' && c != '&'){
            if (!l_system.draw(c)) {
                curr_pos = curr_pos + H;
            }
        }
        else if (c == '+'){
            Vector3D H_temp = H;
            Vector3D L_temp = L;

            H = H_temp * cos(angle) + L_temp * sin(angle);
            L = -H_temp * sin(angle) + L_temp * cos(angle);
        }


        else if (c == '-'){
            Vector3D H_temp = H;
            Vector3D L_temp = L;

            H = H_temp * cos(-angle) + L_temp * sin(-angle);
            L = -H_temp * sin(-angle) + L_temp * cos(-angle);
        }

        else if (c == '^'){
            Vector3D H_temp = H;
            Vector3D U_temp = U;

            H = H_temp * cos(angle) + U_temp * sin(angle);
            U = -H_temp * sin(angle) + U_temp * cos(angle);
        }

        else if (c == '&'){
            Vector3D H_temp = H;
            Vector3D U_temp = U;

            H = H_temp * cos(-angle) + U_temp * sin(-angle);
            U = -H_temp * sin(-angle) + U_temp * cos(-angle);
        }

        else if (c == '\\'){
            Vector3D L_temp = L;
            Vector3D U_temp = U;

            L = L_temp * cos(angle) - U_temp * sin(angle);
            U = L_temp * sin(angle) + U_temp * cos(angle);
        }

        else if (c == '/'){
            Vector3D L_temp = L;
            Vector3D U_temp = U;

            L = L_temp * cos(-angle) - U_temp * sin(-angle);
            U = L_temp * sin(-angle) + U_temp * cos(-angle);
        }

        else if (c == '|'){
            H = -H;
            L = -L;
        }

        else if (c == '('){
            tuple<Vector3D,Vector3D,Vector3D,Vector3D> t (curr_pos,H,L,U);
            stack.push_back(t);
        }
        else if (c == ')'){
            curr_pos = get<0>(stack.back());
            H = get<1>(stack.back());
            L = get<2>(stack.back());
            U = get<3>(stack.back());
            stack.pop_back();
            figure.points.push_back(curr_pos);
        }
    }

    Face face;
    for (unsigned int i = 0; i < figure.points.size(); i++){
        face.point_indexes.push_back(i);
    }
    figure.faces = {face};

    return figure;
}