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
#include "2DL.cpp"
#include "3D-Lijntekeningen.cpp"
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
        Lines2D lines = drawLSystem(l_system(filename),color);
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

        vector<vector<tuple<int,int>>> final_point_indexes;
        Figures3D figures;


        for (int i = 0; i < nrFigures; i++){
            vector<tuple<int,int>> point_indexes;
            string s = to_string(i);
            string fig = "Figure" + s;

            double
            rX = configuration[fig]["rotateX"].as_double_or_die(),
            rY = configuration[fig ]["rotateY"].as_double_or_die(),
            rZ = configuration[fig]["rotateZ"].as_double_or_die(),
            scale = configuration[fig]["scale"].as_double_or_die();

            vector<double>
            center = configuration[fig]["center"].as_double_tuple_or_die(),
            col = configuration[fig]["color"].as_double_tuple_or_die();

            tuple <double,double,double> color;
            get<0>(color) = col[0] * 255;
            get<1>(color) = col[1] * 255;
            get<2>(color) = col[2] * 255;

            int nrPoints = configuration[fig]["nrPoints"].as_int_or_die();
            Figure figure = Figure();
            Face face;


            for (int j = 0; j< nrPoints; j++){

                face.point_indexes.push_back(i);

                string toStr = to_string(j);
                string strPoint = "point" + toStr;

                vector<double> p = configuration[fig][strPoint].as_double_tuple_or_die();

                Vector3D newPoint = Vector3D::point(p[0],p[1],p[2]);

                figure.points.push_back(newPoint);
            }

            figure.faces.push_back(face);
            figure.Color = {get<0>(color),get<1>(color),get<2>(color)};

            applyTransformation(figure,scaleFigure(scale));
            applyTransformation(figure,rotateX(rX));
            applyTransformation(figure,rotateX(rY));
            applyTransformation(figure,rotateX(rZ));

            int nrLines = configuration[fig]["nrLines"].as_int_or_die();

            for (int j = 0; j < nrLines; j++){
                string toStr = to_string(j);
                string strLine = "line" + toStr;

                vector<int> k = configuration[fig][strLine].as_int_tuple_or_die();

                tuple<int,int> indexes;
                get<0>(indexes) = k[0];
                get<1>(indexes) = k[1];

                point_indexes.push_back(indexes);
            }
            figures.push_back(figure);
            final_point_indexes.push_back(point_indexes);
        }
        Lines2D lines = doProjection(figures,final_point_indexes,eye);
        return draw2DLines(lines,1000,background_color);
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
