//
// Created by Mahdi on 12/13/23.
//

#ifndef PROJECT3_FILTERS_H
#define PROJECT3_FILTERS_H
#include "vector"
#include "bmp.h"
#include <algorithm>
#include "chrono"

template <typename T>
T clamp(T value, T min, T max) {
    return std::max(min, std::min(value, max));
}

#define WITHE Pixel(255 , 255 , 255)
#define BLACK Pixel(0 , 0 , 0)




struct FilterThreadArgs {
    FilterThreadArgs(std::vector<std::vector<Pixel>>& pixels_,const std::vector<std::vector<Pixel>>& originalPixels_,const std::vector<std::vector<double>>& coeffs_, int startRow_, int endRow_)
    :pixels(pixels_),
    originalPixels(originalPixels_),
    coeffs(coeffs_),
    startRow(startRow_),
    endRow(endRow_) {}

    int startRow;
    int endRow;
    std::vector<std::vector<Pixel>> &pixels;
    const std::vector<std::vector<Pixel>> &originalPixels;
    const std::vector<std::vector<double>> &coeffs;
};



int kernelSum (const std::vector<std::vector <double>>&ker);
Pixel kernel(std::vector<std::vector<double>>& ker ,const std::vector<std::vector<Pixel>>& frame);
int kernelSum (std::vector<std::vector <double>>&ker);
std::vector<std::vector<Pixel>> submatrix (std::vector<std::vector<Pixel>> &matrix , int x , int y ,std::vector<std::vector<double>> &kernel);


class filters {
public:
    filters(std::vector<std::vector<Pixel>>& pixels_ );
    std::vector<std::vector<Pixel>> applyFilters(std::vector<double> &exectionTime);




private:

    void mirror();
    std::vector<std::vector<Pixel>> pixels;

    
    void convolution(std::vector<std::vector<double>> &kernel , int numThreads);
    std::vector<std::vector<double>> gaussianBlurKernel = {
            {1 , 2 , 1},
            {2 , 4 , 2},
            {1 , 2 , 1}
    };

    void purpleHaze(std::vector<std::vector<double>> &coeffs , int numThreads);
    std::vector<std::vector<double>> purpleHazeCoeffs = {
        {0.16 , 0.5 , 0.16},
        {0.6 , 0.2 , 0.8},
        {0.5 , 0.3 , 0.5}
    };


    void diagonalHatch(int startX , int startY);
    
    void threeParallelhatch();



};


#endif //PROJECT3_FILTERS_H
