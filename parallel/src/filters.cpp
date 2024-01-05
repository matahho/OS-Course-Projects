#include "include/filters.h"
#include<unistd.h> 

filters::filters(std::vector <std::vector<Pixel>> &pixels_) {
    pixels = pixels_;

}
void filters::mirror() {
    for (int r=0 ; r < rows/2 ; r++)
        std::swap(pixels[r] , pixels[rows-r-1]);


    
}

Pixel kernel(const std::vector<std::vector<double>> &ker ,const std::vector<std::vector<Pixel>> &frame){
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


std::vector<std::vector<Pixel>> submatrix(std::vector<std::vector<Pixel>> &matrix, int x, int y, const std::vector<std::vector<double>> &ker) {
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



int kernelSum (const std::vector<std::vector <double>>&ker){
    int sumation = 0 ;
    for (int i = 0 ; i < ker.size() ; i++)
        for (int j = 0 ; j < ker[i].size() ; j++)
            sumation += ker[i][j];
    return sumation;
}


void* partialConvolution(void* args) {
    FilterThreadArgs* args_struct = static_cast<FilterThreadArgs*>(args);
    auto& startRow = args_struct->startRow;
    auto& endRow = args_struct->endRow;
    auto& pixels = args_struct->pixels;
    auto& original_bmp = args_struct->originalPixels;
    auto& ker = args_struct->coeffs;

    for (int i = startRow ; i < endRow ; i++)
        for (int j = 0 ; j < cols ; j++)
            pixels[i][j] = kernel(ker , submatrix(pixels , i , j , ker));




    return NULL;
}



void filters::convolution(std::vector<std::vector<double>>& ker , int numThreads){
    
    auto do_partial = [this, &ker](std::vector<std::vector<Pixel>>& pixels, int row_begin, int row_end) {
        FilterThreadArgs* args = new FilterThreadArgs(pixels,pixels, ker, row_begin, row_end);
        pthread_t t_id;
        pthread_create(&t_id, nullptr, partialConvolution , (void*)args);
        return t_id;
    };

    int rowsPerThread = rows/numThreads;
    std::vector<pthread_t> t_ids;


    for (int i = 0 ; i < numThreads ; i++)  {
        int startR = i * rowsPerThread;
        int endR = (i==(numThreads-1) ? rows :(i + 1) * rowsPerThread);
        
        t_ids.push_back(do_partial(pixels , startR , endR));

    }
    if (cols % numThreads) {
        int row_begin = (rows - (rows % numThreads));
        int row_end = rows;
        t_ids.push_back(do_partial(pixels, row_begin, row_end));
    }
    
    for (auto t_id : t_ids)
        pthread_join(t_id, NULL);



}









void* partialPurpleHaze(void* args) {
    FilterThreadArgs* args_struct = static_cast<FilterThreadArgs*>(args);
    auto& startRow = args_struct->startRow;
    auto& endRow = args_struct->endRow;
    auto& pixels = args_struct->pixels;
    auto& original_bmp = args_struct->originalPixels;
    auto& coeffs = args_struct->coeffs;




    for (int i = startRow; i < endRow; i++) {
        for (int j = 0; j < cols; j++) {

            int newRed = coeffs[0][0] * pixels[i][j].red +
                          coeffs[0][1] * pixels[i][j].green +
                          coeffs[0][2] * pixels[i][j].blue;
            int newGreen = coeffs[1][0] * pixels[i][j].red +
                            coeffs[1][1] * pixels[i][j].green +
                            coeffs[1][2] * pixels[i][j].blue;
            int newBlue = coeffs[2][0] * pixels[i][j].red +
                           coeffs[2][1] * pixels[i][j].green +
                           coeffs[2][2] * pixels[i][j].blue;

            newRed = clamp(static_cast<unsigned int>(newRed), 0u, 255u);
            newGreen = clamp(static_cast<unsigned int>(newGreen), 0u, 255u);
            newBlue = clamp(static_cast<unsigned int>(newBlue), 0u, 255u);

            // Update pixel values after all calculations
            pixels[i][j].red = newRed;
            pixels[i][j].green = newGreen;
            pixels[i][j].blue = newBlue;
        }
    }

    return NULL;
}




void filters::purpleHaze(std::vector<std::vector<double>> &coeffs , int numThreads) {

    auto do_partial = [this, &coeffs](std::vector<std::vector<Pixel>>& pixels, int row_begin, int row_end) {
        FilterThreadArgs* args = new FilterThreadArgs(pixels,pixels, coeffs, row_begin, row_end);

        pthread_t t_id;
        pthread_create(&t_id, nullptr, partialPurpleHaze, (void*)args);
        return t_id;
    };

    int rowsPerThread = rows/numThreads;
    std::vector<pthread_t> t_ids;


    for (int i = 0 ; i < numThreads ; i++)  {
        int startR = i * rowsPerThread;
        int endR = (i==(numThreads-1) ? rows :(i + 1) * rowsPerThread);
        
        t_ids.push_back(do_partial(pixels , startR , endR));

    }
    if (cols % numThreads) {
        int row_begin = (rows - (rows % numThreads));
        int row_end = rows;
        t_ids.push_back(do_partial(pixels, row_begin, row_end));
    }
    
    for (auto t_id : t_ids)
        pthread_join(t_id, NULL);

}








void filters::diagonalHatch(int startX , int startY){
    double slope = static_cast<double>(cols)/static_cast<double>(rows);
    for (int i = startX ; i < rows ; i++)
        pixels[i][int(i*slope) + startY] = WITHE;
}

void filters::threeParallelhatch(){
    diagonalHatch( 0 , cols);
    diagonalHatch( 0 , cols/2);
    diagonalHatch( rows/2 , cols);

}


std::vector<std::vector<Pixel>> filters::applyFilters(std::vector<double> &exectionTime){
    //Flip fiter

    
    auto start = std::chrono::high_resolution_clock::now();
    //mirror();
    auto stop = std::chrono::high_resolution_clock::now();
    exectionTime.push_back(std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count());

    //Blur filter
    start = std::chrono::high_resolution_clock::now();
    int threadsForConvolution = 8 ;
    convolution(gaussianBlurKernel , threadsForConvolution);
    stop = std::chrono::high_resolution_clock::now();
    exectionTime.push_back(std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count());

    //Purple Haze filter
    start = std::chrono::high_resolution_clock::now();
    int threadsForPurpleHaze = 8 ;
    purpleHaze(purpleHazeCoeffs , threadsForPurpleHaze);
    stop = std::chrono::high_resolution_clock::now();
    exectionTime.push_back(std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count());

    //Hatch filters
    start = std::chrono::high_resolution_clock::now();
    threeParallelhatch();
    stop = std::chrono::high_resolution_clock::now();
    exectionTime.push_back(std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count());

    return pixels;
}




