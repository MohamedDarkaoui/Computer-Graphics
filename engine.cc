#include "easy_image/easy_image.h"
#include "ini_config/ini_configuration.h"
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <list>
#include <tuple>
#include <cmath>
#include <vector>
#include "2D/2DL.cpp"
#include "2D/Draw_2D_lines.cpp"
//#include "3D/3D-Lijntekeningen.cpp"
#include "3D/3D-Lichamen.cpp"
#include "3D/3DL.cpp"
#include "ZBuffering_Lines/ZBuffering_lines.h"
#include "ZBuffering_Lines/ZBuffering_lines.cpp"


using namespace std;


img::EasyImage generate_image(const ini::Configuration &configuration){
    //2DL
    if (configuration["General"]["type"].as_string_or_die() == "2DLSystem") {
        string filename = configuration["2DLSystem"]["inputfile"].as_string_or_die();
        vector<double> col = configuration["2DLSystem"]["color"].as_double_tuple_or_die();
        vector<double> backcol = configuration["General"]["backgroundcolor"].as_double_tuple_or_die();

        tuple <double,double,double> color;
        get<0>(color) = col[0] * 255;
        get<1>(color) = col[1] * 255;
        get<2>(color) = col[2] * 255;

        tuple <double,double,double> background_color;
        get<0>(background_color) = backcol[0] * 255;
        get<1>(background_color) = backcol[1] * 255;
        get<2>(background_color) = backcol[2] * 255;
        int size  =  configuration["General"]["size"].as_int_or_die();
        Lines2D lines = draw2DLSystem(l_system(filename),color);
        return draw2DLines(lines , size, background_color);
    }

    //3D lijntekeningen
    else if (configuration["General"]["type"].as_string_or_die() == "Wireframe"){

        vector<double> backcol = configuration["General"]["backgroundcolor"].as_double_tuple_or_die();
        tuple <double,double,double> background_color;
        get<0>(background_color) = backcol[0] * 255;
        get<1>(background_color) = backcol[1] * 255;
        get<2>(background_color) = backcol[2] * 255;

        vector<double> eyee = configuration["General"]["eye"].as_double_tuple_or_die();
        Vector3D eye;
        eye.x = eyee[0];
        eye.y = eyee[1];
        eye.z = eyee[2];
        int nrFigures =  configuration["General"]["nrFigures"].as_int_or_die();


        Figures3D figures;

        for (int i = 0; i < nrFigures; i++) {
            string s = to_string(i);
            string fig = "Figure" + s;

            double rX = configuration[fig]["rotateX"].as_double_or_die();
            double rY = configuration[fig]["rotateY"].as_double_or_die();
            double rZ = configuration[fig]["rotateZ"].as_double_or_die();
            double scale = configuration[fig]["scale"].as_double_or_die();

            double pi = 3.14159265359;

            rX *= pi/180;
            rY *= pi/180;
            rZ *= pi/180;

            vector<double>
                    cent = configuration[fig]["center"].as_double_tuple_or_die(),
                    col = configuration[fig]["color"].as_double_tuple_or_die();

            Vector3D center;
            center.x = cent[0];
            center.y = cent[1];
            center.z = cent[2];

            tuple<double, double, double> color;
            get<0>(color) = col[0] * 255;
            get<1>(color) = col[1] * 255;
            get<2>(color) = col[2] * 255;

            Figure figure = Figure();


            if (configuration[fig]["type"].as_string_or_die() == "LineDrawing") {

                int nrPoints = configuration[fig]["nrPoints"].as_int_or_die();

                for (int j = 0; j < nrPoints; j++) {

                    string toStr = to_string(j);
                    string strPoint = "point" + toStr;

                    vector<double> p = configuration[fig][strPoint].as_double_tuple_or_die();

                    Vector3D newPoint = Vector3D::point(p[0], p[1], p[2]);

                    figure.points.push_back(newPoint);
                }


                int nrLines = configuration[fig]["nrLines"].as_int_or_die();

                for (int j = 0; j < nrLines; j++) {
                    Face face;
                    string toStr = to_string(j);
                    string strLine = "line" + toStr;

                    vector<int> k = configuration[fig][strLine].as_int_tuple_or_die();

                    for (int l : k) {
                        face.point_indexes.push_back(l);
                    }
                    figure.faces.push_back(face);
                }

                figure.draw_extra_line = false;
            }
            else if (configuration[fig]["type"].as_string_or_die() == "Tetrahedron"){
                figure = createTetrahedron();
            }
            else if (configuration[fig]["type"].as_string_or_die() == "Cube"){
                figure = createCube();
            }
            else if (configuration[fig]["type"].as_string_or_die() == "Octahedron"){
                figure = createOctahedron();
            }
            else if (configuration[fig]["type"].as_string_or_die() == "Icosahedron"){
                figure = createIcosahedron();
            }
            else if (configuration[fig]["type"].as_string_or_die() == "Dodecahedron"){
                figure = createDodecahedron();
            }
            else if (configuration[fig]["type"].as_string_or_die() == "Sphere"){
                int n = configuration[fig]["n"].as_int_or_die();
                figure = createSphere(n);
            }
            else if (configuration[fig]["type"].as_string_or_die() == "Cone"){
                int n = configuration[fig]["n"].as_int_or_die();
                double h = configuration[fig]["height"].as_double_or_die();
                figure = createCone(n,h);
            }
            else if (configuration[fig]["type"].as_string_or_die() == "Cylinder"){
                int n = configuration[fig]["n"].as_int_or_die();
                double h = configuration[fig]["height"].as_double_or_die();
                figure = createCylinder(n,h);
            }
            else if (configuration[fig]["type"].as_string_or_die() == "Torus"){
                int n = configuration[fig]["n"].as_int_or_die();
                int m = configuration[fig]["m"].as_int_or_die();
                double r = configuration[fig]["r"].as_double_or_die();
                double R = configuration[fig]["R"].as_double_or_die();

                figure = createTorus(r,R,n,m);
            }
            else if (configuration[fig]["type"].as_string_or_die() == "3DLSystem"){
                string filename = configuration[fig]["inputfile"].as_string_or_die();
                figure = draw3DLSystem(l3D_system(filename));
                figure.draw_extra_line = false;
            }

            figure.Color = {get<0>(color), get<1>(color), get<2>(color)};

            applyTransformation(figure, scaleFigure(scale));
            applyTransformation(figure, rotateX(rX));
            applyTransformation(figure, rotateY(rY));
            applyTransformation(figure, rotateZ(rZ));
            applyTransformation(figure,translate(center));

            figures.push_back(figure);
        }

        int size  =  configuration["General"]["size"].as_int_or_die();
        Lines2D lines = doProjection(figures,eye);
        return draw2DLines(lines,size,background_color);
    }

    else if (configuration["General"]["type"].as_string_or_die() == "ZBufferedWireframe") {

        int size = configuration["General"]["size"].as_int_or_die();

        vector<double> backcol = configuration["General"]["backgroundcolor"].as_double_tuple_or_die();
        tuple<double, double, double> background_color;
        get<0>(background_color) = backcol[0] * 255;
        get<1>(background_color) = backcol[1] * 255;
        get<2>(background_color) = backcol[2] * 255;

        vector<double> eyee = configuration["General"]["eye"].as_double_tuple_or_die();
        Vector3D eye;
        eye.x = eyee[0];
        eye.y = eyee[1];
        eye.z = eyee[2];
        int nrFigures = configuration["General"]["nrFigures"].as_int_or_die();


        Figures3D figures;

        for (int i = 0; i < nrFigures; i++) {
            string s = to_string(i);
            string fig = "Figure" + s;

            double rX = configuration[fig]["rotateX"].as_double_or_die();
            double rY = configuration[fig]["rotateY"].as_double_or_die();
            double rZ = configuration[fig]["rotateZ"].as_double_or_die();
            double scale = configuration[fig]["scale"].as_double_or_die();

            double pi = 3.14159265359;

            rX *= pi / 180;
            rY *= pi / 180;
            rZ *= pi / 180;

            vector<double>
                    cent = configuration[fig]["center"].as_double_tuple_or_die(),
                    col = configuration[fig]["color"].as_double_tuple_or_die();

            Vector3D center;
            center.x = cent[0];
            center.y = cent[1];
            center.z = cent[2];

            tuple<double, double, double> color;
            get<0>(color) = col[0] * 255;
            get<1>(color) = col[1] * 255;
            get<2>(color) = col[2] * 255;

            Figure figure = Figure();


            if (configuration[fig]["type"].as_string_or_die() == "LineDrawing") {

                int nrPoints = configuration[fig]["nrPoints"].as_int_or_die();

                for (int j = 0; j < nrPoints; j++) {

                    string toStr = to_string(j);
                    string strPoint = "point" + toStr;

                    vector<double> p = configuration[fig][strPoint].as_double_tuple_or_die();

                    Vector3D newPoint = Vector3D::point(p[0], p[1], p[2]);

                    figure.points.push_back(newPoint);
                }


                int nrLines = configuration[fig]["nrLines"].as_int_or_die();

                for (int j = 0; j < nrLines; j++) {
                    Face face;
                    string toStr = to_string(j);
                    string strLine = "line" + toStr;

                    vector<int> k = configuration[fig][strLine].as_int_tuple_or_die();

                    for (int l : k) {
                        face.point_indexes.push_back(l);
                    }
                    figure.faces.push_back(face);
                }

                figure.draw_extra_line = false;
            }
            else if (configuration[fig]["type"].as_string_or_die() == "Tetrahedron"){
                figure = createTetrahedron();
            }
            else if (configuration[fig]["type"].as_string_or_die() == "Cube"){
                figure = createCube();
            }
            else if (configuration[fig]["type"].as_string_or_die() == "Octahedron"){
                figure = createOctahedron();
            }
            else if (configuration[fig]["type"].as_string_or_die() == "Icosahedron"){
                figure = createIcosahedron();
            }
            else if (configuration[fig]["type"].as_string_or_die() == "Dodecahedron"){
                figure = createDodecahedron();
            }
            else if (configuration[fig]["type"].as_string_or_die() == "Sphere"){
                int n = configuration[fig]["n"].as_int_or_die();
                figure = createSphere(n);
            }
            else if (configuration[fig]["type"].as_string_or_die() == "Cone"){
                int n = configuration[fig]["n"].as_int_or_die();
                double h = configuration[fig]["height"].as_double_or_die();
                figure = createCone(n,h);
            }
            else if (configuration[fig]["type"].as_string_or_die() == "Cylinder"){
                int n = configuration[fig]["n"].as_int_or_die();
                double h = configuration[fig]["height"].as_double_or_die();
                figure = createCylinder(n,h);
            }
            else if (configuration[fig]["type"].as_string_or_die() == "Torus"){
                int n = configuration[fig]["n"].as_int_or_die();
                int m = configuration[fig]["m"].as_int_or_die();
                double r = configuration[fig]["r"].as_double_or_die();
                double R = configuration[fig]["R"].as_double_or_die();

                figure = createTorus(r,R,n,m);
            }
            else if (configuration[fig]["type"].as_string_or_die() == "3DLSystem"){
                string filename = configuration[fig]["inputfile"].as_string_or_die();
                figure = draw3DLSystem(l3D_system(filename));
                figure.draw_extra_line = false;
            }

            figure.Color = {get<0>(color), get<1>(color), get<2>(color)};

            applyTransformation(figure, scaleFigure(scale));
            applyTransformation(figure, rotateX(rX));
            applyTransformation(figure, rotateY(rY));
            applyTransformation(figure, rotateZ(rZ));
            applyTransformation(figure,translate(center));

            figures.push_back(figure);
        }

        Lines2D lines = doProjection(figures,eye);
        double x1,x2,y1,y2,height,width,x3,y3;
        imageSize(lines,size,x1,x2,y1,y2,width,height,x3,y3);

        ZBuffer zBuffer = ZBuffer((int)width,(int)height);

        return  draw_Zbuf_Lines(lines,size,background_color,zBuffer);
        //return draw2DLines(lines, size,background_color);
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
