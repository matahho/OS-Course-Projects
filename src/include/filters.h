//
// Created by Mahdi on 12/13/23.
//

#ifndef PROJECT3_FILTERS_H
#define PROJECT3_FILTERS_H
#include "vector"
#include "bmp.h"
#include <algorithm>

template <typename T>
T clamp(T value, T min, T max) {
    return std::max(min, std::min(value, max));
}




class filters {
public:
    filters(std::vector<std::vector<Pixel>>& pixels_ );
    std::vector<std::vector<Pixel>> applyFilters();

    std::vector<std::vector<Pixel>> submatrix (std::vector<std::vector<Pixel>> &matrix , int x , int y ,std::vector<std::vector<double>> &kernel);
    std::vector<std::vector<double>> gaussianBlurKernel = {
            {1 , 2 , 1},
            {2 , 4 , 2},
            {1 , 2 , 1}
    };

    void convolution(std::vector<std::vector<double>> &kernel);

private:
    std::vector<std::vector<Pixel>> pixels;

    void mirror();

    int kernelSum (std::vector<std::vector <double>>&ker);
    
    Pixel kernel(std::vector<std::vector<double>>& ker ,const std::vector<std::vector<Pixel>>& frame);

};


#endif //PROJECT3_FILTERS_H
