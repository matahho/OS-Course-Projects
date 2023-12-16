//
// Created by Mahdi on 12/13/23.
//
#include "include/bmp.h"
#include "include/filters.h"

int main(int argc, char* argv[]) {
    std::vector<char> fileBuffer;
    int bufferSize;
    if (!fillAndAllocate(fileBuffer, argv[1], rows, cols, bufferSize)) {
        std::cout << "File read error" << std::endl;
        return 1;
    }

    // Read input file and fill the pixels vector
    std::vector<std::vector<Pixel>> pixels = getPixelsFromBMP24(bufferSize, rows, cols, fileBuffer);

    filters f = filters(pixels);
    std::cout << pixels[349][298].red << std::endl;
    std::cout << pixels[349][298].green << std::endl;
    std::cout << pixels[349][298].blue << std::endl;

    // std::vector<std::vector<Pixel>> matrix = {
    //         {Pixel(10 , 10 , 10),Pixel(20 , 20 , 20), Pixel(30 , 30 , 30)},
    //         {Pixel(40 , 40 , 40), Pixel(50 , 50 , 50), Pixel(60 , 60 , 60)},
    //         {Pixel(70 , 70 , 70), Pixel(80 , 80 , 80), Pixel(90 , 90 , 90)}
    //     };

    // filters f = filters(matrix);

    // // std::vector<std::vector<Pixel>> x = f.submatrix(matrix ,  , 1 ,f.gaussianBlurKernel);
    // for (int i=0 ; i < x.size() ; i++){
    //     for (int j =0 ; j < x[i].size() ; j++)
    //         std::cout << x[i][j].red  << "  ";
    //     std::cout << std::endl;
    // }


    std::vector<std::vector<Pixel>> modified = f.applyFilters();
    std::cout << modified[349][298].red << std::endl;
    std::cout << modified[349][298].green << std::endl;
    std::cout << modified[349][298].blue << std::endl;

    // for (int i=0 ; i < modified.size() ; i++){
    //     for (int j =0 ; j < modified[i].size() ; j++)
    //         std::cout << modified[i][j].red  << "  ";
    //     std::cout << std::endl;
    // }

    // Write output file
    writeOutBmp24(fileBuffer, "output1.bmp", bufferSize , modified);


    return 0;
}