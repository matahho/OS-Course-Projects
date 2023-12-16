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

#define WITHE Pixel(255 , 255 , 255)
#define BLACK Pixel(0 , 0 , 0)



class filters {
public:
    filters(std::vector<std::vector<Pixel>>& pixels_ );
    std::vector<std::vector<Pixel>> applyFilters();



private:
    std::vector<std::vector<Pixel>> pixels;

    void mirror();

    int kernelSum (std::vector<std::vector <double>>&ker);
    std::vector<std::vector<Pixel>> submatrix (std::vector<std::vector<Pixel>> &matrix , int x , int y ,std::vector<std::vector<double>> &kernel);
    std::vector<std::vector<double>> gaussianBlurKernel = {
            {1 , 2 , 1},
            {2 , 4 , 2},
            {1 , 2 , 1}
    };

    void convolution(std::vector<std::vector<double>> &kernel);
    Pixel kernel(std::vector<std::vector<double>>& ker ,const std::vector<std::vector<Pixel>>& frame);

    void purpleHaze(std::vector<std::vector<double>> &coeffs);
    std::vector<std::vector<double>> purpleHazeCoeffs = {
        {0.16 , 0.5 , 0.16},
        {0.6 , 0.2 , 0.8},
        {0.5 , 0.3 , 0.5}
    };


    void diagonalHatch(int startX , int startY);
    void threeParallelhatch();



};


#endif //PROJECT3_FILTERS_H
