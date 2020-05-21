#ifndef ENGINE_ZBUFFERING_LINES_H
#define ENGINE_ZBUFFERING_LINES_H

#include <vector>
#include "../easy_image/easy_image.h"
#include <limits>

class ZBuffer : public std::vector<std::vector<double>>{

public:
    ZBuffer(int height, int width){
        for (int i = 0; i < height; i++){
            vector<double> v;

            for (int j = 0; j < width; j++) {
                double inf = std::numeric_limits<double>::infinity();;
                v.push_back(inf);
            }
            this->push_back(v);
        }
    }
};


#endif //ENGINE_ZBUFFERING_LINES_H
