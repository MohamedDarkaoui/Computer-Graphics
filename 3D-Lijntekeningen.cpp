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

Matrix ScaleFigure(const double scale){
    Matrix m;
    m (1,1) = scale;
    m (2,2) = scale;
    m (3,3) = scale;
    m (4,4) = 1;

    
    return m;
}