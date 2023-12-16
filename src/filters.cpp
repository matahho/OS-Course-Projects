#include "include/filters.h"


filters::filters(std::vector <std::vector<Pixel>> &pixels_) {
    pixels = pixels_;
}
void filters::mirror() {
    for (int r=0 ; r < rows/2 ; r++)
        std::swap(pixels[r] , pixels[rows-r-1]);
    
}

Pixel filters::kernel(std::vector<std::vector<double>> &ker ,const std::vector<std::vector<Pixel>> &frame){
    Pixel result(0 ,0,0);
    if (ker.size() != frame.size()) {
        std::cout << "Error : Kernel Frame does not match to input" << std::endl;
        return Pixel(0 , 0 , 0);
    }
    for (int i = 0 ; i < ker.size() ; i++)
        if (ker[i].size() != frame[i].size()){
            std::cout << "Error : Kernel Frame does not match to input" << std::endl;
            return Pixel(0 , 0 , 0);
        }

    for(int i = 0 ; i < ker.size() ; i++){
        for (int j = 0 ; j < ker[i].size() ; j++){
            result.red += frame[i][j].red * ker[i][j];
            result.green += frame[i][j].green * ker[i][j];
            result.blue += frame[i][j].blue * ker[i][j];

        }
    }
    int normalizeValue = kernelSum(ker);
    result.red /= normalizeValue;
    result.green /= normalizeValue;
    result.blue /= normalizeValue;
    
    result.red = clamp(result.red, 0u, 255u);
    result.green = clamp(result.green, 0u, 255u);
    result.blue = clamp(result.blue, 0u, 255u);

    return result ;
}


std::vector<std::vector<Pixel>> filters::submatrix(std::vector<std::vector<Pixel>> &matrix, int x, int y, std::vector<std::vector<double>> &ker) {
    int size = ker.size();
    int picSizeX = matrix.size();
    int picSizeY = matrix[0].size();

    std::vector<std::vector<Pixel>> sub(size, std::vector<Pixel>(size));

    int startX = x - size / 2;
    int startY = y - size / 2;

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            int posX = std::max(0, std::min(startX + i, picSizeX - 1));
            int posY = std::max(0, std::min(startY + j, picSizeY - 1));

            sub[i][j] = matrix[posX][posY];
        }
    }

    // Handle the edges by filling with the center pixel value
    if (x == 0) {
        for (int i = 0; i < size; i++) {
            sub[0][i] = matrix[x][y];
        }
    }
    if (x == picSizeX - 1) {
        for (int i = 0; i < size; i++) {
            sub[size - 1][i] = matrix[x][y];
        }
    }
    if (y == 0) {
        for (int j = 0; j < size; j++) {
            sub[j][0] = matrix[x][y];
        }
    }
    if (y == picSizeY - 1) {
        for (int j = 0; j < size; j++) {
            sub[j][size - 1] = matrix[x][y];
        }
    }

    return sub;
}





void filters::convolution(std::vector<std::vector<double>>& ker ){


    for (int i = 0 ; i < rows ; i++)
        for (int j = 0 ; j < cols ; j++)
            pixels[i][j] = kernel(ker , submatrix(pixels , i , j , ker));
        

}



int filters::kernelSum (std::vector<std::vector <double>>&ker){
    int sumation = 0 ;
    for (int i = 0 ; i < ker.size() ; i++)
        for (int j = 0 ; j < ker[i].size() ; j++)
            sumation += ker[i][j];
    return sumation;
}







std::vector<std::vector<Pixel>> filters::applyFilters() {
    mirror();
    convolution(gaussianBlurKernel);
    return pixels;
}




